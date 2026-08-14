#!/usr/bin/env python3
"""
Emit results/leaderboard.json in the exact schema of
leaderboard/data/leaderboard.json (schema_version 0.1.0), populated from real
results instead of the shipped dummy data.

Their dummy payload models an *edit* task (fpx / dataflow) with a 4-stage
breakdown (parse/compile/tb/synth). Our run is the *generation* task
(hls_gen_zero_shot), and synthesis was skipped (no Vitis) -- so:

  - task_id follows the pattern "gen_zero_shot_<suite>", one per benchmark
    suite actually run (matches their one-task-per-family convention)
  - breakdown carries pass_parse / pass_compile / pass_tb, with
    pass_synth explicitly null (not omitted -- honest gap, not silence)
  - "score" is defined as pass_tb (the functional-correctness rate). Their
    own dummy score doesn't reduce to a clean function of the breakdown
    (checked: not a mean, not equal to any single field), so rather than
    guess their internal formula, this uses the one number that means
    "the model's output actually worked," which is the most defensible
    single scalar for a generation task missing its 4th stage.

    uv run python scripts/emit_leaderboard.py \
        --raw results/raw --manifest results/run_manifest.json \
        --schema-ref path/to/hls-eval/leaderboard/data/leaderboard.json \
        --out results/leaderboard.json
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path

from aggregate_results import pass_at_k, load_manifest_models  # same dir


def collect_by_suite(raw_dir: Path, allowed_models_norm: set[str]):
    """-> {suite: {model_norm: {design: {"parse":(n,c), "compile":(n,c), "run":(n,c)}}}}"""
    out: dict[str, dict[str, dict[str, dict]]] = {}
    skipped = []

    for d in sorted(raw_dir.iterdir()):
        if not d.is_dir():
            continue
        f = d / "all_eval_data.json"
        if not f.is_file():
            continue
        data = json.loads(f.read_text())
        samples = list(data.values()) if isinstance(data, dict) else data
        if not samples:
            continue
        model_norm = samples[0]["model_name_normalized"]
        design = samples[0]["benchmark_case_name"]
        suite = (samples[0].get("benchmark_case_tags") or ["unknown"])[0]

        if model_norm not in allowed_models_norm:
            skipped.append(d.name)
            continue

        n = len(samples)
        c_parse = sum(1 for s in samples if s.get("can_parse_output"))
        c_compile = sum(
            1 for s in samples
            if (s.get("c_compile_out") or {}).get("data_execution", {}).get("return_code") == 0
        )
        c_run = sum(
            1 for s in samples
            if (s.get("c_run_out") or {}).get("data_execution", {}).get("return_code") == 0
        )

        out.setdefault(suite, {}).setdefault(model_norm, {})[design] = {
            "parse": (n, c_parse), "compile": (n, c_compile), "run": (n, c_run),
        }

    return out, skipped


MODEL_LABELS = {
    "deepseek_deepseek_chat_v3_0324": ("DeepSeek V3 (0324)", "deepseek/deepseek-chat-v3-0324"),
    "deepseek_deepseek_v4_pro": ("DeepSeek V4 Pro", "deepseek/deepseek-v4-pro"),
    "qwen_qwen3_235b_a22b_thinking_2507": ("Qwen3 235B Thinking (2507)", "qwen/qwen3-235b-a22b-thinking-2507"),
    "qwen_qwen3_coder": ("Qwen3 Coder", "qwen/qwen3-coder"),
}
SUITE_LABELS = {
    "polybench": "Zero-Shot Generation: PolyBench",
    "machsuite": "Zero-Shot Generation: MachSuite",
    "c2hlsc": "Zero-Shot Generation: C2HLSC",
}


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--raw", default="results/raw")
    ap.add_argument("--manifest", default="results/run_manifest.json")
    ap.add_argument("--out", default="results/leaderboard.json")
    ap.add_argument("--k", type=int, nargs="+", default=[1, 5])
    ap.add_argument("--generated-at", default=None)
    args = ap.parse_args()

    raw_dir = Path(args.raw)
    manifest = json.loads(Path(args.manifest).read_text())
    allowed = set(load_manifest_models(Path(args.manifest)))

    by_suite, skipped = collect_by_suite(raw_dir, allowed)
    if skipped:
        print(f"skipped {len(skipped)} dirs not in the frozen roster: {skipped}")

    models_seen = sorted({m for suite in by_suite.values() for m in suite})

    payload = {
        "schema_version": "0.1.0",
        "generated_at": args.generated_at or manifest["started_utc"][:10],
        "source": {
            "eval_runs": ["hls_gen_zero_shot__main (reproduction, stock C-sim backend, OpenRouter)"],
            "note": (
                "Real data from a from-scratch reproduction run, not the shipped dummy "
                "payload. Deviations from the paper: (1) provider is OpenRouter, not "
                "Together AI -- serving stack/quantization not controlled, see report; "
                "(2) synthesis (pass_synth) was skipped -- Vitis HLS is not installed, "
                "so that stage is null for every row rather than fabricated; (3) "
                "qwen/qwen-2.5-coder-32b-instruct was dropped from the anchor set after "
                "isolating a single-provider (Cloudflare) failure that reproduced on every "
                "real-prompt request variant -- see report for the debug trail; (4) "
                "max_tokens=8192 truncated a meaningful share of completions from longer "
                "outputs, especially reasoning/verbose models -- treat parse/compile/run "
                "rates as a lower bound on model capability, not solely a capability score."
            ),
        },
        "metrics": [{"id": "pass_rate", "label": "Pass Rate (avg over benchmarks)"}],
        "tasks": [
            {
                "id": f"gen_zero_shot_{suite}",
                "label": SUITE_LABELS.get(suite, f"Zero-Shot Generation: {suite}"),
                "tags": ["gen", "zero_shot", suite],
            }
            for suite in sorted(by_suite)
        ],
        "models": [
            {
                "id": m,
                "label": MODEL_LABELS.get(m, (m, m))[0],
                "provider": "openrouter",
                "model_name": MODEL_LABELS.get(m, (m, m))[1],
            }
            for m in models_seen
        ],
        "results": [],
    }

    for suite in sorted(by_suite):
        task_id = f"gen_zero_shot_{suite}"
        k_values = []
        for k in args.k:
            runs = []
            for model_norm in sorted(by_suite[suite]):
                by_design = by_suite[suite][model_norm]
                agg = {}
                for metric in ("parse", "compile", "run"):
                    vals = [pass_at_k(*stats[metric], k) for stats in by_design.values()]
                    agg[metric] = sum(vals) / len(vals)
                label = MODEL_LABELS.get(model_norm, (model_norm, model_norm))[0]
                runs.append({
                    "model_id": model_norm,
                    "model_label": label,
                    "score": round(agg["run"], 4),
                    "breakdown": {
                        "pass_parse": round(agg["parse"], 4),
                        "pass_compile": round(agg["compile"], 4),
                        "pass_tb": round(agg["run"], 4),
                        "pass_synth": None,
                    },
                })
            k_values.append({"k": k, "runs": runs})
        payload["results"].append({
            "task_id": task_id,
            "metrics": [{"metric_id": "pass_rate", "k_values": k_values}],
        })

    out_path = Path(args.out)
    out_path.write_text(json.dumps(payload, indent=2) + "\n")
    print(f"wrote {out_path}")
    print(f"tasks: {[t['id'] for t in payload['tasks']]}")
    print(f"models: {[m['id'] for m in payload['models']]}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
