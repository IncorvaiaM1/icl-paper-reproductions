#!/usr/bin/env python3
"""
Aggregate raw HLS-Eval results into pass@k with bootstrap CIs, plus figures.

Reads every results/raw/<design>__<model>/all_eval_data.json, computes
parseable / compilable / runnable pass@1 and pass@5 per (design, model),
then aggregates to a per-model mean over designs with a bootstrap 95% CI
(resampling designs with replacement -- the paper's benchmark set is small,
so design-level resampling is the honest source of uncertainty, not
sample-level).

Synthesis is out of scope (Vitis not installed, NullSynthTool everywhere) --
this only ever reports 3 of the paper's 4 staged metrics.

    uv run python scripts/aggregate_results.py \
        --raw results/raw \
        --manifest results/run_manifest.json \
        --out results

Only models listed in run_manifest.json's "models" block are aggregated --
this is what excludes stray pre-freeze smoke-test/debug data (e.g. the
dropped qwen-2.5-coder-32b-instruct anchor) automatically, with no manual
filtering step.
"""

from __future__ import annotations

import argparse
import json
import math
import random
from pathlib import Path

N_BOOTSTRAP = 10_000
SEED = 0


def pass_at_k(n: int, c: int, k: int) -> float:
    """Unbiased pass@k estimator (Chen et al. 2021 / HumanEval)."""
    if n - c < k:
        return 1.0
    # 1 - C(n-c, k) / C(n, k), via product form to avoid overflow
    prod = 1.0
    for i in range(n - c + 1, n + 1):
        prod *= 1 - k / i
    return 1.0 - prod


def load_manifest_models(manifest_path: Path) -> list[str]:
    m = json.loads(manifest_path.read_text())
    return [entry["id"].replace("/", "_").replace("-", "_").replace(".", "_")
            for entry in m["models"]]


def normalize(model_id: str) -> str:
    return model_id.replace("/", "_").replace("-", "_").replace(".", "_")


def collect(raw_dir: Path, allowed_models_norm: set[str]) -> dict:
    """
    -> {model_norm: {design: {"parse": (n,c), "compile": (n,c), "run": (n,c)}}}
    """
    out: dict[str, dict[str, dict[str, tuple[int, int]]]] = {}
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

        out.setdefault(model_norm, {})[design] = {
            "parse": (n, c_parse),
            "compile": (n, c_compile),
            "run": (n, c_run),
        }

    return out, skipped


def bootstrap_ci(per_design_values: list[float], n_boot: int, seed: int) -> tuple[float, float, float]:
    rng = random.Random(seed)
    mean = sum(per_design_values) / len(per_design_values)
    if len(per_design_values) < 2:
        return mean, mean, mean
    boots = []
    n = len(per_design_values)
    for _ in range(n_boot):
        sample = [per_design_values[rng.randrange(n)] for _ in range(n)]
        boots.append(sum(sample) / n)
    boots.sort()
    lo = boots[int(0.025 * n_boot)]
    hi = boots[int(0.975 * n_boot) - 1]
    return mean, lo, hi


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--raw", default="results/raw")
    ap.add_argument("--manifest", default="results/run_manifest.json")
    ap.add_argument("--out", default="results")
    ap.add_argument("--figures", default="figures")
    ap.add_argument("--k", type=int, nargs="+", default=[1, 5])
    args = ap.parse_args()

    raw_dir = Path(args.raw)
    manifest_path = Path(args.manifest)
    out_dir = Path(args.out)
    fig_dir = Path(args.figures)
    fig_dir.mkdir(parents=True, exist_ok=True)

    allowed = set(load_manifest_models(manifest_path))
    per_model, skipped = collect(raw_dir, allowed)

    if skipped:
        print(f"skipped {len(skipped)} dirs not in the frozen roster "
              f"(stray pre-freeze data): {skipped[:5]}{'...' if len(skipped) > 5 else ''}")

    report: dict[str, dict] = {}
    rows_for_csv = ["model,metric,k,mean,ci_low,ci_high,n_designs"]

    for model_norm, by_design in sorted(per_model.items()):
        n_designs = len(by_design)
        report[model_norm] = {"n_designs": n_designs, "metrics": {}}
        for metric in ("parse", "compile", "run"):
            report[model_norm]["metrics"][metric] = {}
            for k in args.k:
                per_design_pk = []
                for design, stats in by_design.items():
                    n, c = stats[metric]
                    per_design_pk.append(pass_at_k(n, c, k))
                mean, lo, hi = bootstrap_ci(per_design_pk, N_BOOTSTRAP, SEED)
                report[model_norm]["metrics"][metric][f"pass@{k}"] = {
                    "mean": round(mean, 4),
                    "ci95_low": round(lo, 4),
                    "ci95_high": round(hi, 4),
                }
                rows_for_csv.append(
                    f"{model_norm},{metric},{k},{mean:.4f},{lo:.4f},{hi:.4f},{n_designs}"
                )

    (out_dir / "aggregate.json").write_text(json.dumps(report, indent=2))
    (out_dir / "aggregate.csv").write_text("\n".join(rows_for_csv) + "\n")
    print(f"wrote {out_dir/'aggregate.json'} and {out_dir/'aggregate.csv'}")

    # ---------------------------------------------------------------- figure
    try:
        import matplotlib
        matplotlib.use("Agg")
        import matplotlib.pyplot as plt

        models = sorted(report.keys())
        metrics = ["parse", "compile", "run"]
        k = args.k[0]  # headline figure uses the first k (typically 1)

        x = range(len(models))
        width = 0.25
        fig, ax = plt.subplots(figsize=(9, 5))
        for i, metric in enumerate(metrics):
            means = [report[m]["metrics"][metric][f"pass@{k}"]["mean"] for m in models]
            los = [report[m]["metrics"][metric][f"pass@{k}"]["ci95_low"] for m in models]
            his = [report[m]["metrics"][metric][f"pass@{k}"]["ci95_high"] for m in models]
            errs = [[mn - lo for mn, lo in zip(means, los)],
                    [hi - mn for mn, hi in zip(means, his)]]
            xs = [xx + (i - 1) * width for xx in x]
            ax.bar(xs, means, width=width, label=metric, yerr=errs, capsize=3)

        ax.set_xticks(list(x))
        ax.set_xticklabels(models, rotation=25, ha="right")
        ax.set_ylabel(f"pass@{k}")
        ax.set_ylim(0, 1.05)
        ax.set_title(f"HLS-Eval pass@{k} by model (bootstrap 95% CI over designs)")
        ax.legend()
        fig.tight_layout()
        fig_path = fig_dir / "repro_vs_paper.png"
        fig.savefig(fig_path, dpi=150)
        print(f"wrote {fig_path}")
    except ImportError:
        print("matplotlib not available -- skipping figure. "
              "Install it (`uv add matplotlib` or `uv run --with matplotlib ...`) and re-run "
              "if you want the PNG; the JSON/CSV above are already complete.")

    # ------------------------------------------------------------- console
    print("\nsummary (pass@1)")
    print("-" * 78)
    for model_norm in sorted(report.keys()):
        m = report[model_norm]["metrics"]
        print(f"  {model_norm:<42} "
              f"parse {m['parse']['pass@1']['mean']:.2f} "
              f"[{m['parse']['pass@1']['ci95_low']:.2f},{m['parse']['pass@1']['ci95_high']:.2f}]  "
              f"compile {m['compile']['pass@1']['mean']:.2f}  "
              f"run {m['run']['pass@1']['mean']:.2f}  "
              f"(n={report[model_norm]['n_designs']} designs)")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
