# Weekend Plan — Reproducing HLS-Eval (Sharc Lab, ICLAD 2025)

**Target paper:** Abi-Karam & Hao, *HLS-Eval: A Benchmark and Framework for Evaluating
LLMs on High-Level Synthesis Design Tasks*, ICLAD 2025.
[arXiv:2504.12268](https://arxiv.org/abs/2504.12268) · [github.com/sharc-lab/hls-eval](https://github.com/sharc-lab/hls-eval)

**Goal:** reproduce a defensible slice of the paper's main result, then extend it with
models that did not exist when the paper was written. Deliver a short report Callie can
read in five minutes and a repo she can browse in ten.

---

## 1. Why this paper

| Criterion | Fit |
|---|---|
| Matches stated interest (LLMs for hardware, HLS) | Direct hit — it is the lab's LLM-for-HLS entry point |
| Runnable with no FPGA board | Yes — evaluation stops at C-sim and synthesis reports |
| Code quality | ~3,600 LOC, `uv`-managed, readable in one evening |
| Room for a real contribution | High — see §2 |
| Reproduction cost | ~$5–15 in API credits, one weekend of wall time |

**Repo inventory (verified by clone, not by reading the README):**

- 110 benchmark designs: chstone 32, polybench 28, machsuite 18, c2hlsc 14, rosetta 9,
  flowgnn 3, gnnbuilder 3, pp4fpga 3
- Each case ships `kernel.cpp`, `kernel.h`, `kernel_tb.cpp`, `kernel_description.md`,
  `hls_eval_config.toml`, `top.txt`, testbench data
- Main experiment (`hls_eval_experiments/hls_gen_zero_shot__main/exp.py`): 4 models,
  `n_samples=5`, `temperature=0.7`
- Four staged metrics: **parseable → compilable → runnable → synthesizable**, reported as pass@k
- `build_model_remote_openrouter()` already exists in `hls_eval/llms.py`

---

## 2. The two openings

### 2.1 Their leaderboard is unpopulated

`leaderboard/data/leaderboard.json` contains:

```json
"note": "Dummy data structured like aggregated pass@k outputs. Replace with generated JSON."
```

There is a complete HTML/CSS/JS leaderboard front-end (`leaderboard/`) with placeholder
numbers behind it. Producing the first real payload in their exact schema is a
contribution they have explicitly left a slot for.

### 2.2 Every model in the paper is from early 2025

The baseline set — Llama-3-70B, Llama-3-8B, Qwen2.5-Coder-32B, DeepSeek-V3 — predates a
full generation of coding and reasoning models. Nobody has re-run this harness since.

The question **"how much of the LLM-for-HLS gap closed in a year?"** is cheap to answer,
uses their infrastructure unmodified, and produces a result the lab would plausibly want
for their own leaderboard. That is the difference between "I ran your code" and
"I found something with your code."

### 2.3 Free rigor upgrade

The paper reports pass@k as point estimates over the benchmark set. Adding **bootstrap
confidence intervals** across designs costs zero extra API calls and shows you understand
that 15–28 designs is a small sample. Small, honest, and the kind of thing that reads as
research maturity rather than showing off.

---

## 3. Vitis dependency — smaller than it first appears

`hls_eval/tools.py` hardcodes Linux paths (`lnx64/tools/clang-3.9-csynth/bin/clang++`,
`lib/lnx64.o`, `lnx64/lib/csim`), so a *Windows* Vitis install is useless here. The
harness needs Linux.

**But the dependency only binds on the last metric.** Verified empirically:

- Zero polybench designs reference `ap_int`, `ap_fixed`, `hls_stream`, or any `hls::` type
  (25 files repo-wide do, all in rosetta / pp4fpga / flowgnn — none in polybench)
- `polybench/2mm` compiles and runs clean under stock `g++ -std=c++14`, no Vitis present
- `#pragma HLS ...` is ignored as an unknown pragma by a standard compiler, so generated
  code carrying pragmas still builds

So on polybench:

| Metric | Needs Vitis? |
|---|---|
| Parseable | No — any C++ parser |
| Compilable | No — stock `clang++` / `g++` |
| Runnable | No — testbenches are plain C++ |
| **Synthesizable** | **Yes** |

This means the weekend does not hinge on a 60 GB download. Get metrics 1–3 on Saturday
with `apt install clang`, and treat Vitis as an upgrade that adds the fourth.

Run `check_env.ps1` and paste the output back — it reports WSL distros, `vitis_hls` on
both Windows and WSL paths, Xilinx roots, free disk, cores, and toolchain versions.

**If you do install Vitis:** in the AMD Unified Installer choose *Vitis*, then deselect
every device family except one small part. Do not take the default full install.
Budget 1–3 hours, mostly unattended.

---

## 4. Scope tiers

Pick after `check_env.ps1` reports. Every tier produces a publishable result — the
difference is how many metrics deep you get.

| Tier | Precondition | Designs | Models | Samples | Metrics |
|---|---|---|---|---|---|
| **A — Full** | Vitis already in WSL/Linux | polybench 28 | 5 | 5 | all 4 |
| **B — Staged** *(default)* | stock clang Sat AM, Vitis downloading in background | polybench 28 | 5 | 5 | 1–3 Sat, add 4 Sun if install lands |
| **C — Software-only** | Vitis never lands | polybench 28 (+ machsuite 18, c2hlsc 14 if time) | 5 | 5 | 1–3 |

Tier B is the plan of record: it front-loads a guaranteed result and treats synthesis as
upside, rather than betting the weekend on an installer.

Note that C keeps the **full** 28-design set — dropping synthesis buys back enough wall
time that there is no reason to subset. A complete 3-metric result across all of polybench
is a better artifact than a partial 4-metric one on 15 designs.

**Verified, not assumed** (see [FINDINGS.md](FINDINGS.md)): all 28 polybench, all 18
machsuite, and all 14 c2hlsc reference designs compile *and* run at 100% under stock GCC
11.4 with no Xilinx software present — 60 designs, three of the five suites the paper's
main experiment used. Polybench remains the primary suite; machsuite and c2hlsc are
in-reach extras if Saturday's run finishes early.

---

## 5. Model roster

Two anchors from the paper (validates the reproduction) plus three current models
(the extension).

**Anchors — must match the paper:**

- `Qwen/Qwen2.5-Coder-32B-Instruct`
- `deepseek-ai/DeepSeek-V3`

**Extension — 3 slots, fill Saturday from the live OpenRouter catalog:**

- one current frontier closed model
- one current open reasoning model
- one current open coding model

Do not hardcode these names in advance — check what OpenRouter actually serves on the day
and record exact model IDs and the run date in the report.

**Deviation to document:** the paper used Together AI; this run uses OpenRouter. Different
serving stacks and quantization can shift results for nominally identical weights. Say so
in the report rather than letting a reviewer find it.

---

## 6. Schedule

### Saturday

| Block | Work |
|---|---|
| AM-1 | Run `check_env.ps1`. Kick off the Vitis-in-WSL download in the background and then ignore it — nothing on Saturday blocks on it. |
| AM-2 | Read the paper properly. Note the four metrics, the pass@k definition, and the exact numbers in the results table — those are the reproduction targets. |
| AM-3 | Clone the repo, `uv sync`, `pytest tests/`. Read `eval.py`, `tools.py`, `prompts.py`. Get an OpenRouter key and confirm `build_model_remote_openrouter` works with one throwaway call. |
| PM-1 | Add a stock-compiler C-sim backend alongside `VitisHLSCSimTool` (same interface, `clang++` instead of Vitis's clang-3.9, drop the `lnx64` library paths). This is the unlock for metrics 1–3 and is maybe 40 lines. |
| PM-2 | Copy `hls_gen_zero_shot__main/exp.py` to a new experiment dir. Point it at OpenRouter, restrict to polybench, wire in the new C-sim backend. Smoke test: 1 design × 1 model × 1 sample. |
| PM-3 | Launch the full 3-metric run across polybench × 5 models × 5 samples. Monitor, do not babysit. |

### Sunday

| Block | Work |
|---|---|
| AM-1 | Re-run anything that failed on transient network errors — the repo's `exp.py` has a commented `evaluate_design_model_pairs` block built for exactly this. |
| AM-2 | Run their `analyze.py` to get figures in the paper's own style. Add the bootstrap CIs. |
| AM-3 | Emit `leaderboard.json` in their schema from your real results. |
| PM-1 | Write the report (§7). Lead with the delta table: paper's number vs yours vs modern models. |
| PM-2 | Push to a public GitHub repo with a README that reproduces your run in three commands. |
| PM-3 | Draft the email. Short. Link the repo, attach one figure, state one finding and one honest limitation. |

---

## 7. Deliverables

```
hls-eval/
├── report/HLS-Eval-Reproduction.md     # 3-4 pages, the thing she actually reads
├── figures/
│   ├── repro_vs_paper.png              # your numbers against the paper's
│   └── one_year_later.png              # 2025 baseline vs 2026 models
├── results/
│   ├── leaderboard.json                # their schema, real data
│   └── raw/                            # full per-design outputs
├── SETUP-WSL.md                        # the friction you hit, written up for the next person
└── email_draft.md
```

### Report structure

1. **What I reproduced** — exact scope, exact model IDs, exact date, one paragraph
2. **Reproduction result** — your numbers next to the paper's, with deviations explained
3. **One year later** — the extension, with CIs
4. **What broke** — every deviation, failure, and unresolved discrepancy, stated plainly
5. **What I'd do next** — two or three concrete follow-ups

Section 4 is the one that matters most. Advisors calibrate on whether a student
distinguishes "it worked" from "I made it look like it worked." Overclaiming a
reproduction is the single fastest way to fail this test drive.

---

## 8. Risks

| Risk | Mitigation |
|---|---|
| Vitis install eats the weekend | Largely neutralized — metrics 1–3 need only stock clang. Vitis runs in the background as upside. |
| Synthesis is slower than estimated | Ship the 3-metric result and report synthesis on whatever subset finished, labelled as such. |
| Numbers do not match the paper | This is a finding, not a failure — report the gap and hypothesize why (serving stack, model drift, tool version). |
| Repo bugs | 4 open issues exist. Hitting one is normal; a clean bug report is itself a contribution. |
| Scope creep into "beat the paper" | Do not. The ask was preliminary results, not a co-authored improvement. |

---

## 9. Explicitly out of scope

- Improving on the paper's method
- Any FPGA board work
- The HLS *editing* task (fpx, dataflow, loop tiling) — generation only
- chstone / rosetta / machsuite — longer synthesis, no added signal
- Opening a PR unprompted. Offer the leaderboard data in the email and let her ask.
