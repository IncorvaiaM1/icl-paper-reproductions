"""
Make HLS-Eval's model calls reproducible and fault-tolerant.

Three caller-side problems, all invisible in the harness's own output.

1. No generation options are set
--------------------------------
`hls_eval/eval.py` calls:

    llm = model.llm
    r = llm.prompt(prompt=prompt, stream=False, temperature=self.temperature)

`max_tokens` is never supplied. Note also that `hls_eval.llms.Model.settings`
is populated by the `build_model_remote_*` helpers and then never read, so the
`provider` routing argument those helpers accept has no effect at all.

2. Provider errors are recorded as model failures
-------------------------------------------------
Observed on a real run: OpenRouter routed `qwen-2.5-coder-32b-instruct` to
Cloudflare, the generation died mid-function, and the response came back with

    finish_reason   = "error"
    prompt_tokens   = 0
    completion_tokens = 1

with 578 characters of partial code. The harness stored that as
`can_parse_output = False` — identical in the results to a model that ignored
the output format. There is no retry and no detection.

Across 700 calls even a 2% provider error rate silently depresses every metric,
and nothing in the output distinguishes infrastructure flakiness from model
capability. This wrapper retries on `finish_reason == "error"` and surfaces the
count so it can be reported rather than absorbed.

3. Quantization is unpinned
---------------------------
OpenRouter load-balances across providers by price, and providers serve
different quantizations of the same open-weight checkpoint (int4 / fp8 / bf16).
Without pinning, "Qwen2.5-Coder-32B scored X" really means "whatever precision
we happened to be routed to, possibly varying between samples." For a benchmark
comparing models that is a confound, and it is a plausible reason anchor numbers
would fail to match a paper that used a single provider.

Usage
-----
    m = build_model_remote_openrouter(model_id, api_key=key)
    m.llm = with_defaults(
        m.llm,
        max_tokens=8192,
        provider=OPENROUTER_REPRODUCIBLE_ROUTING,
    )
"""

from __future__ import annotations

import sys
import time
from typing import Any

# Provider routing for reproducible benchmarking.
#
#   require_parameters -> only route to providers that honour every parameter
#                         we send, so max_tokens/temperature cannot be
#                         silently ignored
#   allow_fallbacks    -> keep failover where more than one provider exists
#
# Quantization is deliberately NOT pinned here. It is the variable you would
# most like to control, but pinning it breaks availability: checked against the
# live catalog, `qwen/qwen-2.5-coder-32b-instruct` has exactly one endpoint
# (Cloudflare) reporting `quantization: "unknown"`, so any filter matches zero
# endpoints and the request 404s. Pass `quantizations=[...]` explicitly for
# models where `scripts/preflight_models.py` shows a filter would actually
# match, and record the served precision either way.
OPENROUTER_REPRODUCIBLE_ROUTING: dict[str, Any] = {
    "require_parameters": True,
    "allow_fallbacks": True,
}


def response_finish_reason(r: Any) -> str | None:
    """Best-effort extraction of finish_reason without raising."""
    try:
        rj = r.json()
    except Exception:
        return None
    if not isinstance(rj, dict):
        return None
    choices = rj.get("choices") or []
    if choices and isinstance(choices[0], dict):
        return choices[0].get("finish_reason")
    return None


class _ModelWithDefaults:
    """Proxy that supplies default options and retries provider errors.

    Anything the caller passes explicitly wins; this only fills gaps. All other
    attribute access passes through to the wrapped model.
    """

    def __init__(
        self,
        inner: Any,
        retry_on_error: int = 3,
        retry_backoff_s: float = 2.0,
        **defaults: Any,
    ) -> None:
        object.__setattr__(self, "_inner", inner)
        object.__setattr__(self, "_defaults", defaults)
        object.__setattr__(self, "_unsupported", set())
        object.__setattr__(self, "_retries", max(1, retry_on_error))
        object.__setattr__(self, "_backoff", retry_backoff_s)
        object.__setattr__(self, "_stats", {"calls": 0, "provider_errors": 0,
                                            "retries_used": 0, "gave_up": 0})

    # ------------------------------------------------------------------ stats
    @property
    def stats(self) -> dict:
        return dict(object.__getattribute__(self, "_stats"))

    # ------------------------------------------------------------------ call
    def _call_once(self, args: tuple, kwargs: dict) -> Any:
        inner = object.__getattribute__(self, "_inner")
        defaults = object.__getattribute__(self, "_defaults")
        unsupported = object.__getattribute__(self, "_unsupported")

        call_kwargs = dict(kwargs)
        for k, v in defaults.items():
            if k not in unsupported:
                call_kwargs.setdefault(k, v)

        try:
            return inner.prompt(*args, **call_kwargs)
        except Exception as exc:
            # Some llm plugins declare a narrower Options model. Drop the
            # offending key, remember it, and retry rather than losing the run.
            offenders = [
                k for k in list(defaults) if k not in unsupported and k in str(exc)
            ]
            if not offenders:
                raise
            for k in offenders:
                unsupported.add(k)
                call_kwargs.pop(k, None)
                print(
                    f"  [warn] model rejected option {k!r}; continuing without it.",
                    file=sys.stderr,
                )
            return inner.prompt(*args, **call_kwargs)

    def prompt(self, *args: Any, **kwargs: Any) -> Any:
        stats = object.__getattribute__(self, "_stats")
        retries = object.__getattribute__(self, "_retries")
        backoff = object.__getattribute__(self, "_backoff")

        stats["calls"] += 1
        r = None
        for attempt in range(1, retries + 1):
            r = self._call_once(args, kwargs)

            # Force the response so finish_reason is available. eval.py calls
            # _force() again later; it is safe to do here.
            try:
                r._force()
            except Exception:
                return r  # let the caller's own error handling deal with it

            if response_finish_reason(r) != "error":
                return r

            stats["provider_errors"] += 1
            if attempt < retries:
                stats["retries_used"] += 1
                print(
                    f"  [warn] provider returned finish_reason='error' "
                    f"(attempt {attempt}/{retries}); retrying in {backoff:.0f}s",
                    file=sys.stderr,
                )
                time.sleep(backoff)
            else:
                stats["gave_up"] += 1
                print(
                    f"  [warn] provider error persisted after {retries} attempts; "
                    f"this sample is infrastructure failure, NOT a model failure",
                    file=sys.stderr,
                )
        return r

    # ------------------------------------------------------------ passthrough
    def __getattr__(self, name: str) -> Any:
        return getattr(object.__getattribute__(self, "_inner"), name)

    def __setattr__(self, name: str, value: Any) -> None:
        setattr(object.__getattribute__(self, "_inner"), name, value)

    def __repr__(self) -> str:
        inner = object.__getattribute__(self, "_inner")
        defaults = object.__getattribute__(self, "_defaults")
        return f"_ModelWithDefaults({inner!r}, {defaults})"


def with_defaults(
    inner: Any,
    retry_on_error: int = 3,
    retry_backoff_s: float = 2.0,
    **defaults: Any,
) -> _ModelWithDefaults:
    """Wrap an `llm` model with default options and provider-error retries."""
    return _ModelWithDefaults(
        inner, retry_on_error=retry_on_error, retry_backoff_s=retry_backoff_s, **defaults
    )
