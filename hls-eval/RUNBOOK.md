# Runbook — copy/paste commands

Line endings and syntax already verified (LF, all scripts parse clean), so
nothing here should trip on the usual Windows/WSL friction.

---

## Step 1 — Probe Windows

**PowerShell** (normal user, no admin needed):

```powershell
cd C:\Users\micha\MAIStuff\randLab\hls-eval
powershell -ExecutionPolicy Bypass -File .\check_env.ps1
```

Paste the output back to me. The parts that matter: whether WSL exists, whether
`vitis_hls` shows up anywhere, and free disk on C:.

---

## Step 2 — Install WSL *(skip if Step 1 found a distro)*

**PowerShell as Administrator:**

```powershell
wsl --install -d Ubuntu
```

Reboot when prompted, then launch Ubuntu once and set a username/password.

---

## Step 3 — Bootstrap the environment

**In WSL:**

```bash
wsl
```

```bash
cd /mnt/c/Users/micha/MAIStuff/randLab/hls-eval
bash scripts/bootstrap_wsl.sh
```

Takes 3–10 min. Installs clang / uv / git, clones hls-eval, runs `uv sync`,
links `ext/` into the repo, then validates the backend against 60 reference
designs.

**Expected tail:**

```
compiled : 60/60  (100.0%)
ran      : 60/60  (100.0%)
All reference designs compile and run. Backend is trustworthy for this suite.
```

If it asks for a sudo password, that's the apt install — normal.

---

## Step 4 — Set a shortcut variable

Saves typing the long path every time. Add it to `.bashrc` so it survives new
shells:

```bash
echo 'export RL=/mnt/c/Users/micha/MAIStuff/randLab/hls-eval' >> ~/.bashrc
source ~/.bashrc
```

---

## Step 5 — Add your OpenRouter key

Get one at <https://openrouter.ai/keys> (load ~$10; the full sweep is well under
that).

Replace `sk-or-v1-YOURKEY` below with the real key — don't paste this line
verbatim. The `sed` first deletes any existing key line so re-running never
leaves duplicates:

```bash
sed -i '/^OPENROUTER_API_KEY=/d' ~/randlab/hls-eval/.env
echo 'OPENROUTER_API_KEY=sk-or-v1-YOURKEY' >> ~/randlab/hls-eval/.env
```

Verify it took, and that it isn't still a placeholder:

```bash
grep OPENROUTER ~/randlab/hls-eval/.env
```

---

## Step 6 — See what models exist today

All remaining commands run from the repo so `uv` picks up the venv:

```bash
cd ~/randlab/hls-eval
```

Confirm the two anchor models from the paper are still served:

```bash
uv run python $RL/scripts/run_experiment.py --list-models --filter qwen-2.5-coder
uv run python $RL/scripts/run_experiment.py --list-models --filter deepseek
```

Then browse for the modern three. Useful filters:

```bash
uv run python $RL/scripts/run_experiment.py --list-models --filter coder
uv run python $RL/scripts/run_experiment.py --list-models --filter qwen
uv run python $RL/scripts/run_experiment.py --list-models --filter llama
uv run python $RL/scripts/run_experiment.py --list-models | head -60
```

Output is `id / context / $ per M input / $ per M output`.

**Pick three**, aiming for one current frontier closed model, one open reasoning
model, and one open coding model. Paste the IDs into `MODERN_MODELS` near the
top of `scripts/run_experiment.py`:

```python
MODERN_MODELS: list[str] = [
    "vendor/model-a",
    "vendor/model-b",
    "vendor/model-c",
]
```

Send me the shortlist if you want a second opinion on the mix.

---

## Step 7 — Dry run (no API calls, no cost)

```bash
uv run python $RL/scripts/run_experiment.py --suite polybench --n-samples 5 --dry-run
```

Prints designs, models, call count, toolchain, and which metrics are live. Check
the model list and the LLM-call count look right before spending anything.

---

## Step 8 — Smoke test (one call, a few cents)

```bash
uv run python $RL/scripts/run_experiment.py --smoke
```

1 design × 1 model × 1 sample. This is the real end-to-end path — prompt,
generation, code extraction, compile, run. Roughly 30s.

Look for a `Testbench compile return code: 0` line. If that works, the sweep
works.

---

## Step 9 — The sweep

```bash
uv run python $RL/scripts/run_experiment.py \
    --suite polybench \
    --n-samples 5 \
    --temperature 0.7 \
    --jobs 12 --jobs-llm 4 --jobs-csim 8 \
    2>&1 | tee $RL/results/sweep_polybench.log
```

28 designs × 5 models × 5 samples = 700 generations. Expect 30–90 min depending
on provider latency, dominated by the API not by compilation.

Results land in `results/raw/`, with `results/run_manifest.json` recording exact
model IDs, compiler version, repo SHA and timestamp.

If it dies partway, the log is in `results/sweep_polybench.log` — send me the
tail and I'll sort it.

**Optional, if that finishes early** — 32 more designs, both verified 100% clean:

```bash
uv run python $RL/scripts/run_experiment.py \
    --suite machsuite --suite c2hlsc --n-samples 5 \
    2>&1 | tee $RL/results/sweep_extra.log
```

---

## Quick reference

| | |
|---|---|
| Re-validate the backend | `python3 $RL/scripts/verify_backend.py --data ~/randlab/hls-eval/hls_eval_data --all` |
| Reproduction only, no modern models | add `--anchors-only` |
| Force the Vitis-free path even if Vitis is installed | add `--force-no-vitis` |
| One specific model | `--models vendor/model-id` |
| Where results go | `$RL/results/raw/`, manifest at `$RL/results/run_manifest.json` |

---

## If something breaks

**`$RL: unbound variable`** — new shell, run `source ~/.bashrc`.

**`No C++ compiler found`** — `sudo apt install -y clang`.

**`OPENROUTER_API_KEY not set`** — the `.env` must be at
`~/randlab/hls-eval/.env`, not in the project folder.

**`ModuleNotFoundError: No module named 'hls_eval'`** — `uv sync` installs
hls-eval's *dependencies* but not hls-eval itself; upstream's `pyproject.toml`
declares no `[build-system]`, so uv treats it as a virtual project. The script
resolves this by locating the checkout and putting it on `sys.path`. If it still
fires, either run from inside the repo or point at it explicitly:

```bash
cd ~/randlab/hls-eval
# or
export HLS_EVAL_REPO=~/randlab/hls-eval
```

**`ModuleNotFoundError: ext`** — the symlink didn't take. Fix:
`ln -sfn $RL/ext ~/randlab/hls-eval/ext`

**A model 404s** — the ID moved. Re-check with `--list-models`.

**Everything shows synthesis rc -99** — expected without Vitis. Synthesis was
skipped, not failed. Don't report 0% synthesizability.
