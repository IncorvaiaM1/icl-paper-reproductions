# HLS-Eval reproduction

Zero-shot HLS generation from
[Abi-Karam & Hao, ICLAD 2025](https://arxiv.org/abs/2504.12268),
[upstream](https://github.com/sharc-lab/hls-eval) @ `adea9ff`, Vitis-free.

Read the combined note first: [`../report/Hao-Lab-Reproductions.pdf`](../report/Hao-Lab-Reproductions.pdf).
Toolchain audit: [`FINDINGS.md`](FINDINGS.md).

## Results (this run)

57 designs (PolyBench + MachSuite + C2HLSC), `n=5`, `T=0.7`, OpenRouter, no synthesis.

| Model | run@1 | run@5 |
|---|---|---|
| deepseek-chat-v3-0324 (paper anchor) | 46.3% | 63.2% |
| qwen3-coder | **67.4%** | 70.2% |
| qwen3-235b-thinking-2507 | 52.3% | 68.4% |
| deepseek-v4-pro | 39.0% | 61.4% |

Paper DeepSeek-V3: 63.3% run@1 / 65.9% run@5 (94 designs, Together AI, Vitis).
Pass@1 here is a lower bound: 21% of calls hit `finish_reason=length` at `max_tokens=8192`.

- Aggregates + CIs: [`results/aggregate.json`](results/aggregate.json)
- Leaderboard JSON (upstream schema, real numbers): [`results/leaderboard.json`](results/leaderboard.json)
- Figure: [`figures/repro_vs_paper.png`](figures/repro_vs_paper.png)

## Layout

```
ext/stock_tools.py     Vitis-free C-sim (stock clang) + null synth
ext/model_defaults.py  generation options, provider-error retries
scripts/               sweep, aggregate, diagnose_parse, backend check
results/               aggregates only (raw traces are local)
report/                standalone HLS write-up
FINDINGS.md            day-0 audit, before any LLM call
```

## Re-running the sweep

Needs WSL, `clang++`, an OpenRouter key, and a clone of upstream hls-eval
(see `scripts/bootstrap_wsl.sh`). The sweep is optional: the numbers above
are already in `results/`. Raising `max_tokens` and restoring the Qwen2.5
anchor via Together AI are listed as unrun follow-ups in the combined PDF.
