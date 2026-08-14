#!/usr/bin/env python3
"""
HLS-Eval zero-shot generation sweep: reproduction + modern-model extension.

Adapted from `hls_eval_experiments/hls_gen_zero_shot__main/exp.py` with four
changes:

  1. OpenRouter instead of Together AI (one key reaches both the paper's
     original models and current ones).
  2. Stock clang++/g++ C-sim backend, so metrics 1-3 need no Vitis install.
  3. Null synthesis tool when Vitis is absent, so the evaluator - which submits
     a synthesis job unconditionally - does not crash.
  4. A run manifest written next to the results recording exact model IDs,
     compiler version, repo SHA, and timestamp. Without this a reproduction is
     not reproducible.

Usage
-----
    # 1 design x 1 model x 1 sample, ~30s, verifies the whole path
    uv run python scripts/run_experiment.py --smoke

    # the real sweep
    uv run python scripts/run_experiment.py \
        --suite polybench --n-samples 5 --temperature 0.7

    # see what OpenRouter is actually serving today
    uv run python scripts/run_experiment.py --list-models --filter qwen
"""

from __future__ import annotations

import argparse
import json
import os
import platform
import subprocess
import sys
import urllib.request
from datetime import datetime, timezone
from pathlib import Path

PROJECT_DIR = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(PROJECT_DIR))


def _resolve_hls_eval_repo() -> Path:
    """Locate the hls-eval checkout and make `hls_eval` importable.

    `uv sync` does not install hls-eval into its own venv: the upstream
    pyproject.toml declares no `[build-system]`, so uv treats it as a virtual
    project and only installs dependencies. The `hls_eval` package is therefore
    importable only if its parent directory is on sys.path. Upstream's own
    experiment scripts get this for free from where they sit in the tree; a
    script living outside the repo does not.
    """
    candidates: list[Path] = []
    if env := os.environ.get("HLS_EVAL_REPO"):
        candidates.append(Path(env))
    cwd = Path.cwd()
    candidates.extend([cwd, *cwd.parents])
    candidates.append(Path.home() / "randlab" / "hls-eval")

    for c in candidates:
        if (c / "hls_eval" / "__init__.py").is_file():
            return c.resolve()

    sys.exit(
        "Could not locate the hls-eval checkout (no hls_eval/__init__.py found).\n"
        "Fix by either:\n"
        "  cd ~/randlab/hls-eval          # run from inside the repo\n"
        "  export HLS_EVAL_REPO=/path/to/hls-eval"
    )


HLS_EVAL_REPO = _resolve_hls_eval_repo()
sys.path.insert(0, str(HLS_EVAL_REPO))

from dotenv import dotenv_values  # noqa: E402

from ext.model_defaults import (  # noqa: E402
    OPENROUTER_REPRODUCIBLE_ROUTING,
    with_defaults,
)
from ext.stock_tools import NullSynthTool, StockCSimTool, detect_compiler  # noqa: E402
from hls_eval.data import BenchmarkCase, find_benchmark_case_dirs  # noqa: E402
from hls_eval.eval import HLSGenerationZeroShotEvaluator  # noqa: E402
from hls_eval.llms import build_model_remote_openrouter  # noqa: E402
from hls_eval.tools import (  # noqa: E402
    VitisHLSCSimTool,
    VitisHLSSynthTool,
    auto_find_vitis_hls_dir,
)

# --------------------------------------------------------------------------
# Model roster.
#
# ANCHORS are from the paper - they are what makes this a reproduction rather
# than just a benchmark run. Keep at least one.
#
# MODERN is the extension. Do NOT trust these IDs blind: OpenRouter's catalog
# moves. Run --list-models on the day and paste in what is actually served.
# --------------------------------------------------------------------------
ANCHOR_MODELS = [
    # qwen/qwen-2.5-coder-32b-instruct DROPPED 2026-08-12 (NEXT.md step 3).
    # debug_request.py isolated the failure to the real structured prompt
    # itself (short prompt -> stop, real prompt -> finish_reason=error on
    # EVERY variant: no params, +temperature, +max_tokens 8192/4096,
    # +stream, and via the llm SDK). Only provider is Cloudflare (1 provider,
    # unknown quantization per preflight). Not fixable within the 30-min
    # timebox without new bisection tooling, which the plan's stop-rules
    # disallow. Report as a single-provider routing-risk finding, not a
    # blocker. DeepSeek-V3 is now the sole paper anchor.
    #
    # The paper used `deepseek-ai/DeepSeek-V3` on Together AI. Deliberately NOT
    # `deepseek/deepseek-chat` here: that ID is a moving alias which today
    # resolves to a v3.1/v3.2/v4-era model, so using it would silently anchor
    # the "reproduction" to a model two generations newer than the paper's.
    # `-v3-0324` is the closest dated snapshot to the paper's April 2025 run.
    "deepseek/deepseek-chat-v3-0324",
]

MODERN_MODELS: list[str] = [
    # Same-lineage successors to the two anchors, so each delta isolates
    # generational improvement rather than confounding vendor/size/data.
    "qwen/qwen3-coder",        # successor to qwen-2.5-coder-32b
    "deepseek/deepseek-v4-pro",  # successor to DeepSeek-V3
    # Third axis: does test-time reasoning help on a task where correctness is
    # adjudicated by a compiler rather than by a judge model?
    "qwen/qwen3-235b-a22b-thinking-2507",
]

# chstone designs that ship without a testbench: 'runnable' is undefined for
# them, so they are excluded from any run that scores that metric.
NO_TESTBENCH = {
    "adpcm__filtep", "adpcm__filtez", "adpcm__logsch", "adpcm__logscl",
    "adpcm__quantl", "adpcm__scalel", "adpcm__uppol1", "adpcm__uppol2",
    "adpcm__upzero", "df_sub128", "jpeg__YuvToRgb",
    "motion__decode_motion_vector",
}

# Reference designs that need Xilinx headers (ap_int / ap_fixed / hls_stream)
# and therefore cannot be C-simulated by a stock compiler without a Vitis
# include directory.
NEEDS_HLS_HEADERS_SUITES = {"rosetta", "flowgnn", "pp4fpga"}


def list_openrouter_models(api_key: str | None, needle: str | None) -> int:
    # The catalog endpoint is public; auth is optional. Browsing before you have
    # a key is useful, so don't require one here.
    headers = {"Authorization": f"Bearer {api_key}"} if api_key else {}
    req = urllib.request.Request(
        "https://openrouter.ai/api/v1/models",
        headers=headers,
    )
    with urllib.request.urlopen(req, timeout=30) as resp:
        data = json.load(resp)
    rows = data.get("data", [])
    if needle:
        rows = [m for m in rows if needle.lower() in m.get("id", "").lower()]
    rows.sort(key=lambda m: m.get("id", ""))
    print(f"{len(rows)} models\n")
    print(f"{'id':<52} {'ctx':>9}  {'$/M in':>8} {'$/M out':>8}")
    print("-" * 84)
    for m in rows:
        p = m.get("pricing", {}) or {}
        try:
            pin = float(p.get("prompt", 0)) * 1e6
            pout = float(p.get("completion", 0)) * 1e6
        except (TypeError, ValueError):
            pin = pout = 0.0
        print(
            f"{m.get('id', ''):<52} {m.get('context_length', 0):>9,}  "
            f"{pin:>8.2f} {pout:>8.2f}"
        )
    return 0


def write_manifest(path: Path, payload: dict) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2, default=str))
    print(f"\nmanifest -> {path}")


def git_sha(repo: Path) -> str:
    try:
        return subprocess.run(
            ["git", "-C", str(repo), "rev-parse", "--short", "HEAD"],
            capture_output=True, text=True, timeout=10,
        ).stdout.strip() or "unknown"
    except Exception:
        return "unknown"


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--repo", type=Path, default=HLS_EVAL_REPO,
                    help="hls-eval checkout (for hls_eval_data/)")
    ap.add_argument("--suite", action="append", default=[],
                    help="repeatable; default polybench")
    ap.add_argument("--models", nargs="*", default=None,
                    help="override the roster entirely")
    ap.add_argument("--anchors-only", action="store_true",
                    help="reproduction only, skip the modern models")
    ap.add_argument("--n-samples", type=int, default=5)
    ap.add_argument("--temperature", type=float, default=0.7)
    ap.add_argument("--max-tokens", type=int, default=8192,
                    help="upstream never sets this; the provider default "
                         "truncates kernels and looks like a parse failure")
    ap.add_argument("--retries", type=int, default=3,
                    help="retries when a provider returns finish_reason=error")
    ap.add_argument("--no-provider-pinning", action="store_true",
                    help="disable quantization/parameter pinning on OpenRouter "
                         "routing (not recommended: precision then varies "
                         "between samples)")
    ap.add_argument("--jobs", type=int, default=12)
    ap.add_argument("--jobs-llm", type=int, default=4)
    ap.add_argument("--jobs-csim", type=int, default=8)
    ap.add_argument("--jobs-synth", type=int, default=8)
    ap.add_argument("--out", type=Path, default=PROJECT_DIR / "results" / "raw")
    ap.add_argument("--smoke", action="store_true",
                    help="1 design x 1 model x 1 sample")
    ap.add_argument("--force-no-vitis", action="store_true",
                    help="ignore an installed Vitis; metrics 1-3 only")
    ap.add_argument("--list-models", action="store_true")
    ap.add_argument("--filter", default=None, help="substring filter for --list-models")
    ap.add_argument("--dry-run", action="store_true",
                    help="print the plan and exit without calling any API")
    args = ap.parse_args()

    env = {**dotenv_values(args.repo / ".env"), **os.environ}
    api_key = env.get("OPENROUTER_API_KEY")

    if args.list_models:
        return list_openrouter_models(api_key, args.filter)

    data_dir = args.repo / "hls_eval_data"
    if not data_dir.is_dir():
        sys.exit(f"hls_eval_data not found at {data_dir} - pass --repo")

    # ---------------------------------------------------------------- designs
    suites = set(args.suite or ["polybench"])
    cases = [
        BenchmarkCase(d, name=d.name) for d in find_benchmark_case_dirs(data_dir)
    ]
    cases = [c for c in cases if set(c.tags_all) & suites]

    skipped_no_tb = [c.name for c in cases if c.name in NO_TESTBENCH]
    cases = [c for c in cases if c.name not in NO_TESTBENCH]

    if suites & NEEDS_HLS_HEADERS_SUITES:
        print(
            f"warning: {sorted(suites & NEEDS_HLS_HEADERS_SUITES)} contain designs "
            "using ap_int/ap_fixed/hls_stream. Without a Vitis include dir these "
            "will fail to compile for reasons unrelated to model quality.\n"
        )

    # ---------------------------------------------------------------- models
    if args.models is not None:
        model_ids = args.models
    elif args.anchors_only:
        model_ids = list(ANCHOR_MODELS)
    else:
        model_ids = list(ANCHOR_MODELS) + list(MODERN_MODELS)

    if args.smoke:
        cases = cases[:1]
        model_ids = model_ids[:1]
        args.n_samples = 1

    if not cases:
        sys.exit(f"no designs matched suites {sorted(suites)}")
    if not model_ids:
        sys.exit("no models selected - fill MODERN_MODELS or pass --models")

    # ---------------------------------------------------------------- tools
    vitis_dir = None if args.force_no_vitis else auto_find_vitis_hls_dir()
    if vitis_dir:
        csim_tool = VitisHLSCSimTool(vitis_dir)
        synth_tool = VitisHLSSynthTool(vitis_dir)
        metrics = "parse, compile, run, synth (all 4)"
    else:
        csim_tool = StockCSimTool()
        synth_tool = NullSynthTool(
            "Vitis HLS not installed" if not args.force_no_vitis
            else "--force-no-vitis"
        )
        metrics = "parse, compile, run (3 of 4 - synthesis SKIPPED, not failed)"

    compiler = None if vitis_dir else str(detect_compiler())

    # ---------------------------------------------------------------- plan
    n_calls = len(cases) * len(model_ids) * args.n_samples
    print("=" * 72)
    print("HLS-Eval zero-shot generation sweep")
    print("=" * 72)
    print(f"  suites     : {', '.join(sorted(suites))}")
    print(f"  designs    : {len(cases)}")
    if skipped_no_tb:
        print(f"  excluded   : {len(skipped_no_tb)} without testbench "
              f"({', '.join(sorted(skipped_no_tb)[:3])}{'...' if len(skipped_no_tb) > 3 else ''})")
    print(f"  models     : {len(model_ids)}")
    for m in model_ids:
        tag = "anchor" if m in ANCHOR_MODELS else "modern"
        print(f"      [{tag}] {m}")
    print(f"  samples    : {args.n_samples} @ temp {args.temperature}")
    print(f"  LLM calls  : {n_calls}")
    print(f"  toolchain  : {'Vitis @ ' + str(vitis_dir) if vitis_dir else compiler}")
    print(f"  metrics    : {metrics}")
    print(f"  output     : {args.out}")
    print("=" * 72)

    if args.dry_run:
        print("\n--dry-run: stopping before any API call.")
        return 0
    if not api_key:
        sys.exit("OPENROUTER_API_KEY not set (put it in <repo>/.env)")

    args.out.mkdir(parents=True, exist_ok=True)
    started = datetime.now(timezone.utc)

    write_manifest(
        args.out.parent / "run_manifest.json",
        {
            "started_utc": started.isoformat(),
            "suites": sorted(suites),
            "designs": [c.name for c in cases],
            "excluded_no_testbench": sorted(skipped_no_tb),
            "models": [
                {"id": m, "role": "anchor" if m in ANCHOR_MODELS else "modern"}
                for m in model_ids
            ],
            "n_samples": args.n_samples,
            "temperature": args.temperature,
            "provider": "openrouter",
            "provider_note": (
                "The paper used Together AI. Serving stack and quantization can "
                "differ for nominally identical weights; treat anchor deltas with "
                "that in mind."
            ),
            "synthesis": "vitis" if vitis_dir else "SKIPPED (NullSynthTool, rc=-99)",
            "compiler": compiler,
            "vitis_dir": str(vitis_dir) if vitis_dir else None,
            "hls_eval_sha": git_sha(args.repo),
            "python": sys.version.split()[0],
            "platform": platform.platform(),
        },
    )

    # eval.py sets no generation options, never reads Model.settings, and has
    # no retry: a provider-side failure lands in the results as a model failure.
    # See ext/model_defaults.py.
    routing = None if args.no_provider_pinning else dict(OPENROUTER_REPRODUCIBLE_ROUTING)
    models = []
    wrapped = []
    for mid in model_ids:
        m = build_model_remote_openrouter(mid, api_key=api_key)
        opts: dict = {"max_tokens": args.max_tokens}
        if routing:
            opts["provider"] = routing
        m.llm = with_defaults(m.llm, retry_on_error=args.retries, **opts)
        wrapped.append((mid, m.llm))
        models.append(m)

    evaluator = HLSGenerationZeroShotEvaluator(
        vitis_hls_tool_csim=csim_tool,
        vitis_hls_tool_synth=synth_tool,
        output_data_dir=args.out,
        n_samples=args.n_samples,
        temperature=args.temperature,
    )

    evaluator.evaluate_designs(
        benchmark_cases=cases,
        models=models,
        n_jobs=args.jobs,
        n_jobs_pool_llm=args.jobs_llm,
        n_jobs_pool_csim=args.jobs_csim,
        n_jobs_pool_synth=args.jobs_synth,
    )

    elapsed = (datetime.now(timezone.utc) - started).total_seconds()
    print(f"\ndone in {elapsed / 60:.1f} min -> {args.out}")

    # Provider reliability is part of the result, not a hidden detail: without
    # this, infrastructure errors are indistinguishable from model failures.
    totals = {"calls": 0, "provider_errors": 0, "retries_used": 0, "gave_up": 0}
    print("\nprovider reliability")
    print("-" * 72)
    for mid, w in wrapped:
        s = w.stats
        for k in totals:
            totals[k] += s.get(k, 0)
        if s["provider_errors"]:
            print(f"  {mid:<44} errors={s['provider_errors']:<4} "
                  f"retried={s['retries_used']:<4} unrecovered={s['gave_up']}")
    if totals["provider_errors"] == 0:
        print("  no provider errors")
    else:
        print(f"  TOTAL calls={totals['calls']} errors={totals['provider_errors']} "
              f"unrecovered={totals['gave_up']}")
        if totals["gave_up"]:
            print(f"\n  {totals['gave_up']} sample(s) failed for infrastructure "
                  f"reasons, not model capability.\n"
                  f"  Exclude them or report them separately - they are not "
                  f"parseability failures.")
    (args.out.parent / "provider_reliability.json").write_text(
        json.dumps(
            {
                "per_model": {mid: w.stats for mid, w in wrapped},
                "totals": totals,
                "routing": routing,
            },
            indent=2,
        )
    )
    if not vitis_dir:
        print("\nREMINDER: synthesis was skipped (rc=-99), not failed.")
        print("Do not report 0% synthesizability from this run.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
