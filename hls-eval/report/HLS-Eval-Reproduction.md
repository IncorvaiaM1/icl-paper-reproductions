# HLS-Eval Reproduction + One Year Later

**What:** a reproduction of the zero-shot HLS kernel generation baseline from
*HLS-Eval: A Benchmark and Framework for Evaluating LLMs on High-Level
Synthesis Design Tasks* (Abi-Karam & Hao, ICLAD 2025), extended with three
models that postdate the paper.

**Run date:** 2026-08-12. **Repo:** hls-eval @ `adea9ff` (2026-07-30), `uv`-managed, Vitis-free stock C-sim backend at `ext/stock_tools.py`.

---

## 1. What I reproduced

The paper's zero-shot HLS generation task: given a natural-language kernel
description, a header, and a testbench, generate the C++ implementation.
Scored on three of the paper's four staged metrics — **parseable,
compilable, runnable** — at `n_samples=5`, `temperature=0.7`. Synthesis
(the fourth metric) is out of scope: Vitis HLS is not installed on this
machine, so every synthesis cell is recorded as skipped, not failed.

**Scope:** 57 designs across PolyBench, MachSuite, and C2HLSC — the three
suites verified in `FINDINGS.md` to compile and run at 100% under a stock
compiler with zero Xilinx software present. This is 3 short of the 60
designs that verification pass counted; the discrepancy wasn't chased down
(anti-scope-creep call) and is flagged here rather than silently absorbed.

**Models:**

| Role | Model | Provider | Notes |
|---|---|---|---|
| Anchor | `deepseek/deepseek-chat-v3-0324` | OpenRouter (SiliconFlow) | Closest dated snapshot to the paper's `deepseek-ai/DeepSeek-V3`; the alias `deepseek/deepseek-chat` was deliberately avoided — it now resolves to a v3.1/v3.2/v4-era model |
| Extension | `qwen/qwen3-coder` | OpenRouter | |
| Extension | `deepseek/deepseek-v4-pro` | OpenRouter | |
| Extension | `qwen/qwen3-235b-a22b-thinking-2507` | OpenRouter | |
| Dropped anchor | `qwen/qwen-2.5-coder-32b-instruct` | — | See §4 |

**Deviation from the paper:** the paper used Together AI as the sole
provider; this run used OpenRouter, which load-balances across providers
serving different quantizations of nominally the same weights. Numeric
precision is therefore an uncontrolled variable here in a way it wasn't for
the paper — see the preflight table in §4.

1,140 total LLM calls, 2 provider errors, both auto-recovered on retry, 0
unrecovered.

---

## 2. Reproduction result — paper's numbers vs. mine

The paper's anchor table (Table III, zero-shot generation, all 94 designs,
Vitis synthesis included, Together AI):

| Model | parse@1 | parse@5 | compile@1 | compile@5 | run@1 | run@5 |
|---|---|---|---|---|---|---|
| DeepSeek V3 (paper) | 100.0% | 100.0% | 94.1% | 97.6% | **63.3%** | **65.9%** |
| Qwen2.5-Coder-32B (paper) | 95.5% | 97.6% | 90.8% | 92.9% | 57.6% | 58.8% |

My run, same task, 57-design subset, OpenRouter, no synthesis, bootstrap
95% CI over designs in brackets:

| Model | parse@1 | parse@5 | compile@1 | compile@5 | run@1 | run@5 |
|---|---|---|---|---|---|---|
| deepseek-chat-v3-0324 (anchor) | 63.2% [54.0, 72.3] | 87.7% [79.0, 94.7] | 61.4% [52.3, 70.2] | 86.0% [77.2, 94.7] | 46.3% [35.8, 56.8] | **63.2%** [50.9, 75.4] |

**The pass@1 gap is real but the pass@5 gap nearly vanishes**: 63.2% vs. the
paper's reported 65.9% at k=5 on the metric that matters most (runnable —
functional correctness). That pattern — a model recovering almost fully by
the 5th sample but stumbling on the 1st — is the signature of a **budget or
format** problem, not a capability gap. §4 identifies the specific cause:
`max_tokens=8192` truncates a meaningful share of completions before the
closing tag, and the truncated sample counts as a hard failure at k=1 while
the other 4 samples usually still land. Treat the pass@1 numbers above as a
**lower bound**, not a clean capability comparison to the paper.

Three confounds prevent a clean 1:1 comparison, stated plainly:
1. Different benchmark subset — paper's 94 vs. my 57 (no CHStone, no
   Rosetta, both excluded for Xilinx-header/testbench reasons documented in
   `FINDINGS.md`).
2. Different provider — OpenRouter/SiliconFlow vs. Together AI, unpinned
   quantization.
3. `max_tokens=8192` was not part of the paper's stated methodology and
   caps at least 21% of all completions in this run (§4).

**Qwen2.5-Coder-32B-Instruct — not reproduced.** This is the paper's other
anchor. It was dropped from the roster (§4) before the real sweep; its
paper numbers are cited above for reference only, not compared against
anything from this run.

---

## 3. One year later — the extension

| Model | parse@1 | compile@1 | run@1 | run@5 |
|---|---|---|---|---|
| deepseek-chat-v3-0324 (2025 anchor) | 63.2% [54.0, 72.3] | 61.4% [52.3, 70.2] | 46.3% [35.8, 56.8] | 63.2% [50.9, 75.4] |
| qwen3-235b-a22b-thinking-2507 | 73.7% [66.7, 80.4] | 70.9% [62.8, 78.6] | 52.3% [41.4, 63.2] | 68.4% [56.1, 78.9] |
| deepseek-v4-pro | 52.3% [42.5, 62.5] | 49.5% [40.0, 59.3] | 39.0% [29.1, 49.1] | 61.4% [49.1, 73.7] |
| qwen3-coder | **99.7%** [99.0, 100.0] | **96.8%** [93.0, 99.7] | **67.4%** [55.4, 78.6] | **70.2%** [57.9, 80.7] |

**Qwen3-Coder is the clear standout** — parse and compile are both
essentially saturated (99.7%, 96.8%), meaning the harness/task itself, not
the model, is now the bottleneck for those two stages. Its run@1 (67.4%) is
21 points above the 2025 DeepSeek-V3 anchor under identical harness
constraints — a real, defensible "the gap closed" finding.

**Deepseek-v4-pro is the weakest model in this run**, worse than the 2025
anchor on every metric despite being DeepSeek's newer release. This is very
likely an artifact, not a regression: it's one of the two models hit
hardest by the `max_tokens` truncation (§4) — its no-code-extracted rate is
132/285 samples (46.3%), highest of any model tested, and its combined
parse-failure rate (no_tags + odd_tags) is 136/285 (47.7%). A model
being reasoning-heavy or verbose punishes it disproportionately under a
fixed small token budget; this result should not be read as "DeepSeek got
worse."

**Headline for the extension:** one year of model progress did not
translate uniformly into gains on this benchmark under this harness's fixed
`max_tokens=8192` — the newest, most capable models are, if anything, more
exposed to that ceiling than the 2025 baseline was.

---

## 4. What broke

Ordered roughly by how much it should change how the numbers above are
read.

### `max_tokens=8192` truncates a meaningful share of completions

`diagnose_parse.py` over all 1,141 evaluations: 72.1% parsed cleanly,
27.0% had no extractable code tags, 0.9% had malformed tags. Of the whole
run, 21.0% of calls hit `finish_reason=length` — the provider's own signal
that generation was cut off, not that it finished. The tool's own verdict:

> `'odd_tags'` usually means the response was cut off mid-code. That is a
> property of the max-token setting in this run, not of the model's HLS
> ability.

Per-model breakdown makes the asymmetry concrete:

| Model | ok | no_tags | odd_tags | failure rate |
|---|---|---|---|---|
| qwen3-coder | 284 | 0 | 1 | 0.4% |
| qwen3-235b-a22b-thinking-2507 | 210 | 72 | 3 | 26.3% |
| deepseek-chat-v3-0324 | 180 | 104 | 1 | 36.8% |
| deepseek-v4-pro | 149 | 132 | 4 | 47.7% |

Not every failure is a truncation, though: 318 total parse failures vs. 240
`length`-terminated calls means roughly 78 failures completed normally
(`finish_reason=stop`) and still didn't parse — a genuine format/extraction
miss, distinct from the token-budget issue. Both are real; they shouldn't
be conflated in a rerun.

**Fix, not attempted here (harness frozen once the sweep started, per
plan):** raise `max_tokens` to something like 12–16k, especially for
reasoning-tagged models, and rerun. This is the single highest-value follow-up.

### The Qwen2.5-Coder-32B anchor could not be reproduced

Every harness request to `qwen/qwen-2.5-coder-32b-instruct` returned
`finish_reason: "error"` with corrupted output and bogus token accounting
(`prompt_tokens: 0, completion_tokens: 1`), while a hand-written curl to the
same model/provider succeeded. `debug_request.py` bisected this by varying
one factor at a time (prompt content/size, temperature, `max_tokens`,
streaming, SDK vs. raw HTTP): every real-prompt variant failed identically;
only the short synthetic test prompt succeeded. Verdict: the long,
structured HLS-Eval prompt itself breaks this specific
model/provider/serving-stack combination, not a parameter or the SDK.

This model has exactly one OpenRouter provider (Cloudflare) with unknown
quantization — no failover exists. Given the 30-minute timebox in the plan,
this was reported as a finding rather than chased further: **DeepSeek-V3 is
the sole paper anchor in this run.** A Together AI key (the paper's actual
provider) is the natural next step to test whether this is
Cloudflare/OpenRouter-specific.

### Provider/quantization is an uncontrolled variable

| Model | OpenRouter providers | Quantization | Note |
|---|---|---|---|
| qwen-2.5-coder-32b | 1 (Cloudflare) | unknown | no failover — see above |
| deepseek-chat-v3-0324 | 4 | bf16/fp4/fp8 | 88% worst-case uptime |
| qwen3-coder | 5 | fp4/fp8/unknown | healthy |
| deepseek-v4-pro | 18 | fp4/fp8/unknown | 0.14% worst-case uptime on one provider |
| qwen3-235b-thinking | 4 | fp8/unknown | healthy |

The paper used a single provider (Together AI). Reproducing an
open-weights benchmark through a routing layer means precision is
uncontrolled, and for one model, unknowable. This alone would explain a
meaningful slice of any gap versus the paper even with an unlimited token
budget.

### Sweep pace

Estimated 1–2h, actual 388.6 minutes (~6.5h) for 1,140 calls — attributable
to reasoning-model latency (the two "thinking"-tagged/verbose models
dominate wall time). Not a defect, just worth knowing before scheduling a
rerun.

### Reference-design and harness bugs (found before any model was run — see `FINDINGS.md` for full detail)

- 12 of 32 CHStone designs ship with no testbench — the *runnable* metric
  is undefined for them, not failed; scoring them as failures understates
  CHStone runnability by 37.5%.
- `flowgnn/fgnn_linear`'s testbench calls `printf` with no `<cstdio>`
  include — builds under Vitis's bundled clang (which pulls it in
  transitively) but not under a standard compiler.
- `gnnbuilder/gather_node_neighbors`'s testbench writes out of bounds and
  dies to a stack-smashing check under GCC — silent under Vitis's clang,
  which doesn't enable the stack protector by default.
- The harness sets no execution timeout — one generated infinite loop hangs
  a worker for the rest of a sweep.
- Execution inherits the parent process's working directory — any
  testbench doing relative-path file I/O reads from the wrong place.

### The 57-vs-60-design gap

`FINDINGS.md`'s verification pass counted 60 clean designs across these
three suites; the actual sweep manifest lists 57. Not investigated further
— logged as an open, minor discrepancy rather than chased down.

---

## 5. What I'd do next

1. **Re-run with a larger `max_tokens`** (12–16k) — cheap, and the single
   highest-leverage fix given how much of the current gap is truncation,
   not capability.
2. **Add a Together AI key** and re-test the Qwen2.5-Coder-32B anchor on
   the paper's actual provider, to isolate whether the failure is
   OpenRouter/Cloudflare-specific or a genuine harness/model interaction.
3. **Install Vitis** (or a viable open synthesis substitute) to restore the
   fourth metric and make the comparison to the paper's numbers complete
   rather than 3-of-4.
4. **Extend to CHStone and Rosetta** once the testbench-less CHStone
   designs and Xilinx-header dependency in Rosetta are handled, to reach
   the paper's full 94-design scope.

---

## Appendix: run provenance

- hls-eval @ `adea9ff` (2026-07-30)
- Python 3.12.3, `clang++` 18.1.3, Linux 6.6.87.2 (WSL2)
- `n_samples=5`, `temperature=0.7`, provider `openrouter`
- Full manifest: `results/run_manifest.json`
- Raw per-design output: `results/raw/`
- Aggregated pass@k + CIs: `results/aggregate.json`, `results/aggregate.csv`
- Real leaderboard payload (their schema): `results/leaderboard.json`
- Figure: `figures/repro_vs_paper.png`
- Day-0 toolchain findings: `FINDINGS.md`
