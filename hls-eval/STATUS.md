# Status — where we are, what's broken, what's next

_Snapshot before making further changes._

---

## 1. The goal (unchanged)

Reproduce a defensible slice of **HLS-Eval** (Abi-Karam & Hao, ICLAD 2025) and
extend it with models that postdate the paper, then send Callie a short report
and a browsable repo. Weekend budget.

Two things make it worth her time rather than just being homework:

1. Her lab's `leaderboard/data/leaderboard.json` is placeholder data with a note
   saying "Replace with generated JSON." Nobody has filled it.
2. Every model in the paper is from early 2025. Nobody has re-run the harness
   since.

---

## 2. Done and verified

### Environment
| | |
|---|---|
| WSL | Ubuntu 24.04.3, 16 cores, 13 GB RAM, 938 GB free |
| Compiler | clang 18.1.3 |
| Repo | hls-eval @ `adea9ff` (2026-07-30), `uv sync` clean |
| API | OpenRouter key live, $10 cap, $0 used |
| Vitis | **not installed** — synthesis metric unavailable |

### The Vitis-free backend works
`ext/stock_tools.py` replaces Vitis's bundled clang with the host compiler,
unlocking 3 of the paper's 4 metrics with no Xilinx software.

Validated on two independent toolchains:

| Toolchain | Scope | Result |
|---|---|---|
| clang 18.1.3 / Ubuntu 24.04 | polybench + machsuite + c2hlsc | **60/60 compile and run** |
| GCC 11.4 / Ubuntu 22.04 | all 110 designs | 84 compile, 83 run |

Not a quirk of one compiler's leniency.

### Findings already banked (see FINDINGS.md)
These came from running the reference designs, before any model was involved:

- **12 of 32 chstone designs ship without a testbench** — the *runnable* metric
  is undefined for them; scoring them as failures understates chstone
  runnability by 37.5%
- **`flowgnn/fgnn_linear`** testbench calls `printf` with no `<cstdio>` include
- **`gnnbuilder/gather_node_neighbors`** testbench writes out of bounds, dies
  with stack smashing
- **No execution timeout** in the harness — one generated infinite loop hangs a
  worker for the rest of a sweep
- **Execution inherits the parent's cwd** — testbenches doing relative-path file
  I/O read from the wrong directory
- **`Model.settings` is never read** by `eval.py`, so the `provider` argument the
  model builders accept silently does nothing

### Tooling built
```
check_env.ps1              Windows probe
scripts/bootstrap_wsl.sh   one-shot environment setup
scripts/verify_backend.py  validate compiler backend vs reference designs
scripts/preflight_models.py inspect provider/quantization per model
scripts/run_experiment.py  the sweep
scripts/diagnose_parse.py  classify why parseability failed
scripts/debug_request.py   bisect a failing request  <- next to run
ext/stock_tools.py         StockCSimTool + NullSynthTool
ext/model_defaults.py      generation options + provider-error retries
```

---

## 3. The one open blocker

**`qwen/qwen-2.5-coder-32b-instruct` fails through the harness.**

Every attempt returns `finish_reason: "error"` with nonsense accounting
(`prompt_tokens: 0, completion_tokens: 1`) and output corrupted *mid-line*:

```c
for (int i = ; i < n - d; i++)    // the 0 is missing from the middle
```

A hand-written curl to the same model and the same provider succeeds cleanly.
So the request the harness sends differs from a working one in some way we
haven't isolated.

### Ruled out
| Hypothesis | Verdict |
|---|---|
| Response truncated by `max_tokens` | No — `finish_reason` is `error`, not `length` |
| Cloudflare endpoint is broken | No — bare curl to it returns `finish_reason: stop` |
| Quantization pinning would help | No — it caused a 404; that model has no fp16/bf16 endpoint |

Three wrong guesses. Hence `debug_request.py`, which varies one factor at a time
(prompt size → content → temperature → max_tokens → SDK transport) instead of
guessing a fourth.

### Not yet known — and cheap to find out
**Only one model has ever been smoke-tested.** Everything above concerns the
Qwen anchor. DeepSeek and the three modern models are untested. If they work,
this is a Qwen/Cloudflare-specific problem and the sweep is not blocked — only
one calibration point is.

---

## 4. Preflight caveats worth carrying into the report

| Model | Providers | Quantization | Note |
|---|---|---|---|
| qwen-2.5-coder-32b | **1** (Cloudflare) | **unknown** | no failover, precision uncontrollable |
| deepseek-chat-v3-0324 | 4 | bf16/fp4/fp8 | healthy, 88% worst uptime |
| qwen3-coder | 5 | fp4/fp8/unknown | healthy |
| deepseek-v4-pro | 18 | fp4/fp8/unknown | **0.14% worst-case uptime** on one provider |
| qwen3-235b-thinking | 4 | fp8/unknown | healthy |

The general point, which is itself a finding: reproducing an open-weights
benchmark through a routing layer means **numeric precision is an uncontrolled
variable**, and for some models an unknowable one. The paper used a single
provider (Together AI); OpenRouter load-balances by price across providers
serving different quantizations of the same checkpoint.

---

## 5. Decisions pending

**A. The Qwen anchor.** Depends on `debug_request.py`:
- fixable → keep both anchors, no change
- not fixable → either drop to a single DeepSeek anchor (zero setup, one less
  calibration point) or add a Together AI key (paper-faithful, since the paper
  used Together, but another signup)

**B. Synthesis.** Vitis still not installed. Tier B (start the download, add the
4th metric if it lands) vs Tier C (ship 3 metrics across more designs). Given
time already spent debugging, **Tier C is looking like the honest call.**

**C. Scope.** polybench 28 as planned, or the full 60 verified designs
(polybench + machsuite + c2hlsc). Dropping synthesis makes 60 affordable.

---

## 6. Revised plan

| Step | Action | Est. |
|---|---|---|
| 1 | `debug_request.py` — isolate the Qwen failure | 5 min |
| 2 | Smoke-test the other four models — is this Qwen-only? | 10 min |
| 3 | Resolve decision A on that evidence | — |
| 4 | Full sweep, 3 metrics | 1–2 h |
| 5 | `diagnose_parse.py` over results; separate infra failures from model failures | 15 min |
| 6 | Aggregate pass@k + bootstrap CIs; figures | 45 min |
| 7 | Emit real `leaderboard.json` in their schema | 20 min |
| 8 | Write report + email | 1 h |

---

## 7. Worth noting

**The debugging is not lost time.** Six harness/benchmark issues are already
documented, several of which would silently corrupt anyone's results:

- provider errors recorded as model parseability failures, with no retry and no
  detection
- no generation options set, so provider defaults apply unseen
- quantization unpinned and sometimes unknowable
- 12 designs where a scored metric is undefined
- 2 reference-design bugs
- 2 harness robustness gaps

Even if the sweep produced nothing, "here is what breaks when someone outside
your lab runs your benchmark, and here are the fixes" is a real contribution —
arguably more useful to her than another row of numbers.

**The risk to watch** is scope creep into harness engineering. The ask was
preliminary results. The fixes exist to make the numbers trustworthy, not as the
deliverable. Once the sweep runs, stop fixing and start writing.
