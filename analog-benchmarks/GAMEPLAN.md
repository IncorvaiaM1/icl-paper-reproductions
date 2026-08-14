# GAMEPLAN — Analog Benchmarks: reproduce → macromodel library → AMS-Eval

_Written 2026-08-13, in the NEXT.md house style: decisions up front, timeboxes,
stop-rules. Covers (A) reproducing what is reproducible from Hasler & Hao TRETS
2024, (B) the reference macromodel library, (C) AMS-Eval, the LLM benchmark.
B is a prerequisite of both A's endgame and C's grader, so the phases interleave._

**This is the two-weekend stretch plan. For the next couple of days, follow
[`GAMEPLAN_48h.md`](GAMEPLAN_48h.md) instead — Phase C, the FPAA VM, and blocks
5–7 are out of scope.**

**Assumptions (stated, not asked):** same working setup as the HLS-Eval repro
(WSL Ubuntu 24.04, OpenRouter key, no proprietary EDA). Budget: ~2 weekends.
The HLS-Eval email ships first — this is project #2, not a blocker for it.

---

## What "reproduce results" means here — be precise, because the paper has no
## experimental table

This is a benchmark-definition paper, not a measurement paper. There is no
pass@k table to match. The reproducible claims are:

1. **The worked example (§4, Fig 17–18):** acoustic Case II lowers to a
   16–24-band BPF filterbank + 3 delay stages + a 48-in × 128-out VMM+WTA with a
   6,144-weight matrix, costed via Fig 18's CAB table to "more WTA stages than
   fit on one SoC FPAA (barely), two 350 nm SoC FPAAs, ~250 µm × 300 µm in
   130 nm std cells." All of that is arithmetic you can re-derive and check.
2. **The scaling curves (Figs 3 & 11):** fabric bandwidth, FG count, MAC/s vs
   process node, and CABs-per-die / fabric frequency per node. These are
   extrapolations from stated anchor points (350 nm ⇒ 60 MHz, 130 nm ⇒ 500 MHz,
   40 nm ⇒ 6 GHz, 14 nm ⇒ 50 GHz; die sizes 6/25/100 mm²) — re-derivable.
3. **The benchmark solutions themselves (Fig 16):** four ~3-line block programs.
   The paper says every level=1 block "should have a macromodel for potential
   digital simulation" and the suite "could also be solved using digital
   computation." Nobody outside the lab can run these today because the blocks
   have no public executable semantics and the artifact URL is 404. Running
   them end-to-end on the public datasets **is** the reproduction — and it is
   exactly deliverable B.
4. **Prior demonstrated results it leans on:** Case I acoustic classification on
   the SoC FPAA (refs [31, 73–75, 80]) — reproducible only in spirit (digital
   macromodel), since we have no FPAA board. Say so honestly in the report.

---

## Phase 0 — Artifact hunt & ground truth (half day, timeboxed)

| # | Action | Time | Rule |
|---|--------|------|------|
| 0.1 | Wayback Machine: `web.archive.org/web/*/hasler.ece.gatech.edu/AnalogBenchmarks*` — grab any snapshot of the benchmark files | 30 min | Found → it becomes the spec of record. Not found → the 404 is a confirmed finding, move on. |
| 0.2 | Download the ICE-lab FPAA tools Ubuntu VM (hasler.ece.gatech.edu/FPAAtool) and locate the Xcos block palette — it contains the lab's own macromodels for BPF/LadderF/VMM/WTA | 1–2 h (mostly download) | This is the semantic ground truth for Phase B. If a block's behavior is ambiguous in the paper, the VM's Scilab code decides. |
| 0.3 | Pull the two spec-bearing companion papers: JLPEA 13(4):58 (analog HLS, 2023) and TCAS-I 68(2) ladder-filter paper [75] | 30 min | These define LadderF/LadderFM and the synthesis flow the TRETS paper deferred. |
| 0.4 | Datasets: Speech Commands v2 (Case I/II acoustic — pick 4-word and 32-word subsets), MNIST, Free Spoken Digit Dataset as TI-digits stand-in. **TIMIT is LDC-licensed — do not scrape it; note the substitution in the report.** | 30 min | |

**Gate 0:** none of this can fail in a blocking way. Everything downstream
works from the paper's Figs 15–18 alone if 0.1–0.2 come up empty.

---

## Phase A — Reproduction checks (1 day, pure computation, no models)

Repo: `ams-bench/` (new, sibling of the hls-eval work).

| # | Action | Output |
|---|--------|--------|
| A.1 | `analysis/scaling.py` — re-derive Fig 3a–c and Fig 11 curves from the four anchor nodes + die areas; overlay digitized points from the paper's figures; flag any slope that doesn't match | `figures/scaling_check.png` + 5 lines in report |
| A.2 | `amsbench/costs.py` — encode the Fig 18 CAB-cost table as data; function `cab_cost(program) -> {CABs, breakdown}` | reusable cost model (also Phase C's 4th metric) |
| A.3 | Validate A.2 against the §4 worked example: Case II acoustic ⇒ does the arithmetic land on "barely > 1 SoC FPAA, fits in 2"? Document every assumption needed to make it land (e.g., WTA outputs/CAB = 1 on SoC FPAA vs 8–16 for std cells — the paper states both) | one report subsection; any mismatch is a finding, same genre as FINDINGS.md |
| A.4 | Re-check the 40 nm claims quoted for comm Case III (2 GHz met by a 2×3 mm 40 nm FPAA, ref [53]) against the scaling model | consistency note |

**Stop-rule:** A is descriptive. No fixing, no emailing mid-phase. Findings go
in `FINDINGS_ams.md` as text.

---

## Phase B — `amsblocks`: the reference macromodel library (2–3 days — the core build)

Python package, numpy/scipy only. Two fidelity tiers per block, mirroring the
paper's own level=1 abstraction:

- **tier-F (functional):** discrete-time, fast, deterministic — the grading
  backend. e.g., BPF = 2nd-order state-space; VMM = `W @ x`.
- **tier-B (behavioral CT):** `scipy.integrate.solve_ivp` ODE form with optional
  noise/mismatch/Ibias knobs — demonstrates the "analog computation = ODEs"
  thesis and is the honest macromodel. Ship tier-F for all blocks; tier-B for
  the ones the demos need (BPF, LadderF, WTA, NeuronHH).

Block list, in build order (signatures verbatim from Fig 15):

| Priority | Blocks | Semantics source |
|---|---|---|
| 1 | `VMM`, `M2V`, `MergeV` | trivial; Fig 15 |
| 2 | `BPF(centerFreq, Q)`, `AmplitudeBPF` (BPF + rectify + envelope LPF) | Fig 17 circuit + [31] |
| 3 | `VMMWTA(W, Thresh, freq)` — k-WTA; train with softmax surrogate, evaluate with hard WTA | [73, 80, 98] |
| 4 | `LadderF/LadderFM(Ibias1, Ibias2)` — gm-C ladder ⇒ programmable delays | [75] + VM Xcos code |
| 5 | `LPF`, `HopfLPF`, `SigVMM` | Fig 15, [86] |
| 6 | `LinearEqIterate(A, b)` — τ·dx/dt = b − Ax to steady state | [70, 100] |
| 7 | `NeuronHH`, `LocalSepImagerConv`, `ClassifyBlockImage2Vector`, `Scanner`, `VMMDemod` | [101], [33, 74]; stubs acceptable at v0.1 if documented |

**Each block gets:** docstring stating the interpretation + its paper/VM source,
a unit test, and a `cab_cost` entry.

**Milestone B-demo (the money artifact):** `examples/acoustic_case1.py` — WAV →
16-band exponentially-spaced AmplitudeBPF filterbank (50 Hz–10 kHz) → LadderFM
delays → M2V → VMMWTA, weights trained offline on Speech Commands 4-word subset,
accuracy reported on held-out set. This literally executes Fig 16's sample
acoustic code. Then the other three Fig 16 programs at whatever fidelity the
stubs allow (filter benchmark = LadderF sweep → output spectrum → ripple/
stopband/SNR metrics, which needs no dataset at all — do it second, it's cheap
and fully specified in §3.4).

**Gate B:** Case I acoustic ≥ ~90% on 4 words and the filter benchmark meeting
its own spec (<2% ripple, ≥40 dB stopband on the tier-F model). If accuracy
stalls, ship whatever it is with CIs — the deliverable is the executable suite,
not a SOTA number (same rule as NEXT.md stop-rule 4).

---

## Phase C — AMS-Eval: the LLM harness (2–3 days)

Architecture: lift the HLS-Eval harness shape wholesale — per-design directory,
model adapters, n samples, pass@k + bootstrap CIs — with the six known fixes
baked in from day one (execution timeout + address-space cap, cwd set to build
dir, generation options pinned, provider preflight, retry-on-provider-error,
max_tokens ≥ 32k). You already have all this code.

**Task format** (`designs/<case>/`): `spec.md` (benchmark description in the
paper's own terms + I/O contract), `blocks_api.md` (Fig 15 signatures — the
"standard library" the model may call), `eval.py` (dataset harness → accuracy),
`budget.json` (CAB cap from Fig 11/18 for the stated node), reference solution
(hidden from prompts).

**Metric ladder** (the HLS-Eval mirror):

| HLS-Eval | AMS-Eval | Implementation |
|---|---|---|
| parseable | parseable | extract code block; AST parse; calls restricted to the `amsblocks` whitelist |
| compilable | simulatable | runs under tier-F without exception, bounded time/memory |
| runnable | functional | accuracy ≥ threshold on held-out data (e.g., ≥80% Case I acoustic); filter: meets ripple/stopband spec |
| synthesizable | resource-feasible | `cab_cost(program)` ≤ budget |

**Task roster v0 (8 tasks):** acoustic Case I × 3 disjoint 4-word vocab draws
(anti-memorization), acoustic Case II (32 words), filter benchmark at 2
frequency targets, vision MNIST via LocalSepImagerConv+VMMWTA, comm toy
(VMMDemod on a synthetic QAM trace). Sizes are small on purpose — the claim is
"first LLM benchmark for analog system synthesis," not coverage.

**Sweep:** proven roster from the HLS-Eval repro (DeepSeek-V3 anchor +
Qwen3-Coder + deepseek-v4-pro + qwen3-235b-thinking) × 8 tasks × n=5, temp 0.7.
~$5–10 on OpenRouter at these prompt sizes. Aggregate with the existing
pass@k/CI scripts. Emit a `leaderboard.json` in the HLS-Eval schema.

**Gate C:** if models score ~0 across the board, that is itself the headline
result ("frontier LLMs cannot compose analog block programs yet") — report it
with per-rung failure attribution from a `diagnose_parse.py` port. Do not tune
prompts to rescue scores; one prompt revision max, documented.

---

## Deliverables & sequencing

```
Weekend 1: Phase 0 → A → B blocks 1–4 + acoustic Case I demo
Weekend 2: B filter demo + remaining blocks → C harness → sweep → report
```

1. `ams-bench` repo: `amsblocks/` + `analysis/` + `designs/` + `results/`
2. `FINDINGS_ams.md`: dead artifact link (with Wayback evidence), any Fig 17–18
   arithmetic mismatches, block-semantics ambiguities and how the VM resolved them
3. Report (~2 pages, same shape as HLS-Eval-Reproduction.md): what the paper
   claims, what was re-derivable, the executable suite, AMS-Eval v0 numbers
4. Email #2 to Callie (cc/or separately Prof. Hasler — it is her hardware and
   her dead link): repro of *their* benchmark definitions as running code + the
   first LLM eval on top of it. Five sentences + repo link, per the house style.

## Stop-rules (carried over, they worked)

1. No new tooling beyond the named scripts; a problem needing an eighth script
   means drop the thing causing it.
2. Block semantics get ONE interpretation each, documented; no re-litigating.
   VM code > companion papers > paper text, in that order of authority.
3. Anything not landed by end of weekend 2 becomes one sentence of future work.
4. The deliverable is the executable suite + report, not the accuracy numbers.

## Risks

- **Underspecified blocks** (LadderFM delays, WTA dynamics): mitigated by 0.2/0.3;
  worst case, tier-F uses an explicit documented simplification — that's still
  the first public executable semantics, which is the point.
- **Speech pipeline underperforms** without the dynamics tricks ref [75] uses:
  acceptable; report the number, cite the gap.
- **Scope creep into circuit-level fidelity (tier-B everywhere):** the known
  failure mode. Tier-B is demo garnish, not a gate.
- **TIMIT/TI-digits licensing:** substituted datasets, disclosed.
