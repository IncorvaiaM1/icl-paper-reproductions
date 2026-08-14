#!/usr/bin/env python3
"""
Inspect the real serving situation for every model in the roster, before
spending a sweep's worth of API calls on it.

Why this is not optional
------------------------
OpenRouter is a routing layer, not a model host. For open-weight models the
same nominal ID can be served by many providers at different numeric
precisions, or - as with `qwen/qwen-2.5-coder-32b-instruct` - by exactly one
provider at `quantization: "unknown"`.

That matters for a benchmark:

  * single-provider models have no failover, so a provider incident becomes a
    data-quality problem with no workaround
  * unknown/varying quantization is an uncontrolled variable sitting under
    every score, and a plausible reason anchor numbers fail to match a paper
    that used one specific provider
  * a provider that does not list `max_tokens` in supported_parameters will
    silently ignore it

None of this is visible from the model list. It is visible here.

    python scripts/preflight_models.py
    python scripts/preflight_models.py --quantizations fp8 bf16   # test a filter
"""

from __future__ import annotations

import argparse
import json
import sys
import urllib.error
import urllib.request
from pathlib import Path

PROJECT_DIR = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(PROJECT_DIR))

ENDPOINTS_URL = "https://openrouter.ai/api/v1/models/{model}/endpoints"
NEEDED_PARAMS = ("max_tokens", "temperature")


def fetch_endpoints(model_id: str, api_key: str | None) -> dict | None:
    headers = {"Authorization": f"Bearer {api_key}"} if api_key else {}
    req = urllib.request.Request(ENDPOINTS_URL.format(model=model_id), headers=headers)
    try:
        with urllib.request.urlopen(req, timeout=30) as resp:
            return json.load(resp).get("data")
    except urllib.error.HTTPError as e:
        print(f"  !! HTTP {e.code} for {model_id}")
        return None
    except Exception as e:  # noqa: BLE001
        print(f"  !! {type(e).__name__} for {model_id}: {e}")
        return None


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--models", nargs="*", default=None,
                    help="default: the roster in run_experiment.py")
    ap.add_argument("--quantizations", nargs="*", default=None,
                    help="test whether a quantization filter would match anything")
    ap.add_argument("--json", type=Path, default=PROJECT_DIR / "results" / "preflight.json")
    args = ap.parse_args()

    if args.models:
        model_ids = args.models
    else:
        import re
        src = (PROJECT_DIR / "scripts" / "run_experiment.py").read_text()
        ids: list[str] = []
        for block in ("ANCHOR_MODELS", "MODERN_MODELS"):
            m = re.search(rf"{block}[^=]*= \[(.*?)\n\]", src, re.S)
            if m:
                ids += re.findall(r'"([^"#]+/[^"#]+)"', m.group(1))
        model_ids = ids

    if not model_ids:
        sys.exit("no models to check")

    env_key = None
    for env_path in (Path.home() / "randlab" / "hls-eval" / ".env",):
        if env_path.is_file():
            for line in env_path.read_text().splitlines():
                if line.startswith("OPENROUTER_API_KEY="):
                    env_key = line.split("=", 1)[1].strip()

    print(f"checking {len(model_ids)} models\n")
    report = []
    problems: list[str] = []

    for mid in model_ids:
        data = fetch_endpoints(mid, env_key)
        if not data:
            problems.append(f"{mid}: could not resolve - is the ID correct?")
            continue

        eps = data.get("endpoints") or []
        quants = sorted({e.get("quantization") or "unknown" for e in eps})
        providers = [e.get("provider_name") for e in eps]
        max_out = [e.get("max_completion_tokens") for e in eps if e.get("max_completion_tokens")]
        ctx = [e.get("context_length") for e in eps if e.get("context_length")]

        missing_params = sorted({
            p for e in eps for p in NEEDED_PARAMS
            if p not in (e.get("supported_parameters") or [])
        })
        uptimes = [e.get("uptime_last_1d") for e in eps if e.get("uptime_last_1d") is not None]

        print(f"{mid}")
        print(f"  providers    : {len(eps)}  {', '.join(str(p) for p in providers[:6])}")
        print(f"  quantization : {', '.join(quants)}")
        print(f"  context      : {min(ctx) if ctx else '?'}"
              f"   max out: {min(max_out) if max_out else '?'}")
        if uptimes:
            print(f"  uptime (1d)  : {min(uptimes)}% worst of {len(uptimes)}")

        if len(eps) == 1:
            print("  [!] SINGLE PROVIDER - no failover; a provider incident "
                  "corrupts this model's data")
            problems.append(f"{mid}: single provider ({providers[0]})")
        if quants == ["unknown"]:
            print("  [!] quantization UNKNOWN - precision is an uncontrolled variable")
            problems.append(f"{mid}: unknown quantization")
        if missing_params:
            print(f"  [!] some endpoints do not support: {', '.join(missing_params)}")
            problems.append(f"{mid}: endpoints missing {missing_params}")

        if args.quantizations:
            matching = [e for e in eps if (e.get("quantization") or "unknown") in args.quantizations]
            verdict = f"{len(matching)} endpoint(s)" if matching else "NO ENDPOINTS -> 404"
            print(f"  filter {args.quantizations} -> {verdict}")
            if not matching:
                problems.append(f"{mid}: quantization filter would 404")

        print()
        report.append({
            "id": mid, "n_providers": len(eps), "providers": providers,
            "quantizations": quants, "context": ctx, "max_completion_tokens": max_out,
            "missing_params": missing_params,
        })

    args.json.parent.mkdir(parents=True, exist_ok=True)
    args.json.write_text(json.dumps(
        {"models": report, "problems": problems,
         "quantization_filter_tested": args.quantizations}, indent=2))

    print("=" * 72)
    if problems:
        print(f"{len(problems)} issue(s) to note in the report:\n")
        for p in problems:
            print(f"  - {p}")
        print("\nThese are not necessarily blockers. They are caveats that belong")
        print("in the methodology section, because they affect comparability.")
    else:
        print("no issues found")
    print(f"\nwrote {args.json}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
