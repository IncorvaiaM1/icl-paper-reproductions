# HLS-Eval reproduction

Reproducing and extending **Abi-Karam & Hao, _HLS-Eval: A Benchmark and
Framework for Evaluating LLMs on High-Level Synthesis Design Tasks_**, ICLAD 2025.

Sweep, report, findings, and email draft are done. See
[`report/HLS-Eval-Reproduction.md`](report/HLS-Eval-Reproduction.md),
[`FINDINGS.md`](FINDINGS.md), and [`email_draft.md`](email_draft.md). Do not
reopen the harness. Lab index: [`../README.md`](../README.md).

[paper](https://arxiv.org/abs/2504.12268) · [upstream code](https://github.com/sharc-lab/hls-eval)

---

## Status

| | |
|---|---|
| Toolchain audit | done — see [FINDINGS.md](FINDINGS.md) |
| Vitis-free C-sim backend | done — `ext/stock_tools.py`, validated 60/60 |
| Environment probe | ready — `check_env.ps1` |
| WSL bootstrap | ready — `scripts/bootstrap_wsl.sh` |
| Experiment sweep | ready — `scripts/run_experiment.py` |
| Model roster | **needs filling** — `MODERN_MODELS` in `run_experiment.py` |
| Results | not started |
| Report | not started |

---

## Headline result so far

Three of the paper's four metrics need no Vitis HLS installation. **60 of 110
reference designs — all of polybench, machsuite and c2hlsc — compile and run at
100% with stock GCC and nothing from Xilinx.** Only synthesizability genuinely
requires Vitis.

Auditing the reference designs with a modern compiler also surfaced two bugs and
one benchmark property that biases a metric. Details in
[FINDINGS.md](FINDINGS.md).

---

## Layout

```
check_env.ps1              Windows-side probe: WSL, Vitis, disk, toolchain
PLAN.md                    scope, schedule, risks
FINDINGS.md                day-0 audit results
ext/
  stock_tools.py           StockCSimTool + NullSynthTool (Vitis-free backends)
scripts/
  bootstrap_wsl.sh         one-shot WSL environment setup
  verify_backend.py        validate the backend against reference designs
  run_experiment.py        the sweep
results/
  backend_check_all.json   full 110-design audit
  raw/                     per-design evaluation output
report/                    the write-up
figures/                   plots
```

---

## Running it

### 1. Probe Windows

```powershell
powershell -ExecutionPolicy Bypass -File .\check_env.ps1
```

### 2. Bootstrap WSL

```bash
wsl
cd /mnt/c/Users/micha/MAIStuff/randLab/hls-eval
bash scripts/bootstrap_wsl.sh
```

Installs clang / uv / git, clones hls-eval, runs `uv sync`, links `ext/` into
the repo, and validates the backend. Vitis is detected if present but not
required.

### 3. Add an API key

```bash
echo 'OPENROUTER_API_KEY=sk-or-...' >> ~/randlab/hls-eval/.env
```

### 4. Pick the modern models

```bash
cd ~/randlab/hls-eval
uv run python /mnt/c/Users/micha/MAIStuff/randLab/hls-eval/scripts/run_experiment.py \
    --list-models --filter qwen
```

Paste the chosen IDs into `MODERN_MODELS` in `scripts/run_experiment.py`. Do not
trust hardcoded IDs — OpenRouter's catalog moves.

### 5. Smoke test, then sweep

```bash
uv run python .../run_experiment.py --smoke
uv run python .../run_experiment.py --suite polybench --n-samples 5
```

`--dry-run` prints the plan and cost shape without touching an API.

---

## Reading the results

**Synthesis skipped is not synthesis failed.** With no Vitis, `NullSynthTool`
returns `-99`, deliberately neither 0 nor 1. Any aggregation that treats
"nonzero" as failure will report 0% synthesizability, which is false rather than
merely incomplete. Filter the sentinel before aggregating.

**Anchor models exist to validate the reproduction.** `qwen-2.5-coder-32b` and
`deepseek-chat` are in the paper. If their numbers land far from the published
ones, the deviation needs explaining before the modern-model comparison means
anything.

**The provider differs from the paper.** The paper used Together AI; this uses
OpenRouter. Serving stacks and quantization vary for nominally identical
weights. Recorded in every run manifest.
