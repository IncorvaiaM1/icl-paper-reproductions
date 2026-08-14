# GAMEPLAN 48h — analog benchmarks, cut for the lab email window

_Written 2026-08-14. The two-weekend plan in `GAMEPLAN.md` is still the right
end-state. This file is what actually happens in the next couple of days._

**Assumptions:** WSL Ubuntu 24.04, numpy/scipy only, no proprietary EDA, no
FPAA board. HLS-Eval email ships independently — do not block it on this.

---

## Decision (closed)

Ship **executable Fig 16 acoustic Case I** + **re-derived CAB/scaling arithmetic**.
That is the analog equivalent of the HLS-Eval reproduction: you ran their
benchmark definition as code, and you checked the numbers they published.

**Drop from this window (one sentence of future work each):**

| Drop | Why |
|---|---|
| Phase C — AMS-Eval LLM sweep | Novel, correctly the pitch, 2–3 days *after* the macromodels exist |
| FPAA Ubuntu VM download | Gate 0 already says non-blocking; a multi-GB VM will eat the window |
| Blocks 5–7 (NeuronHH, imager, …) | Stubs. Case I does not need them |
| Case II 32-word, MNIST, comm toy | Stretch demos |
| SmartPAF / CRYPTONITE | Separate papers; wrong substrate / too heavy for 48h |

**Novel-but-not-crazy, for the email only:** AMS-Eval (HLS-Eval metric ladder on
Fig 15 blocks). Write it as the proposed first project. Do not implement it.

---

## Hour budget

```
Hour 0     folder org (done) + freeze this file
Hours 0.5  Phase 0.1 Wayback + 0.3 pull PDFs  (skip 0.2 VM)
Hours 1–4  Phase A: scaling.py, costs.py, Case II CAB check
Hours 4–12 Phase B blocks 1–3 + LadderFM delay as a documented simplification
Hours 12–16 examples/acoustic_case1.py on Speech Commands 4-word
Hours 16–18 filter demo IF Case I is running; else skip
Hours 18–20 FINDINGS_ams.md + 1–2 page report
Hour 20    one paragraph for the HLS-Eval email (trajectory), not a second email
```

Anything not landed at hour 20 becomes one sentence of future work. Same stop
rule that made HLS-Eval shippable.

---

## Phase 0 — ground truth (timeboxed, nothing here can block)

| # | Action | Time | Rule |
|---|--------|------|------|
| 0.1 | Wayback: `web.archive.org/web/*/hasler.ece.gatech.edu/AnalogBenchmarks*` | 20 min | Found → spec of record. Empty → confirmed finding, move on. |
| 0.2 | **SKIP** the ICE-lab FPAA VM | — | Revisit only if a block is genuinely uninterpretable |
| 0.3 | PDFs: JLPEA 13(4):58 (open access) + TRETS if not already local | 20 min | Save under `analog-benchmarks/papers/` |
| 0.4 | Speech Commands v2, 4-word subset only (`yes/no/up/down` or similar). No TIMIT. | 20 min | MNIST/FSDD deferred |

**Gate 0:** Figs 15–18 of TRETS are enough to start Phase A even if 0.1 is empty.

---

## Phase A — reproduction checks (pure computation)

Repo root for code: `analog-benchmarks/ams-bench/` (created in this phase).

| # | Action | Output |
|---|--------|--------|
| A.1 | `analysis/scaling.py` — re-derive Fig 3 / Fig 11 from the four anchor nodes (350 nm ⇒ 60 MHz, 130 nm ⇒ 500 MHz, 40 nm ⇒ 6 GHz, 14 nm ⇒ 50 GHz) and die areas 6/25/100 mm² | `figures/scaling_check.png` + 5 lines in the report |
| A.2 | `amsblocks/costs.py` — Fig 18 CAB table as data; `cab_cost(program)` | reusable cost model |
| A.3 | Case II acoustic arithmetic: 16–24-band BPF + 3 delays + 48×128 VMM+WTA (6,144 weights). Does it land on "barely > 1 SoC FPAA, fits in 2"? Document every assumption | `FINDINGS_ams.md` subsection |
| A.4 | 40 nm comm Case III consistency note against the scaling model | one paragraph, skip if A.3 overruns |

**Stop-rule:** A is descriptive. Mismatches are findings, not bugs to "fix."

---

## Phase B — just enough `amsblocks` to run Fig 16 acoustic

numpy/scipy. **Tier-F only** (discrete-time, deterministic). No ODEs this window.

| Priority | Blocks | Notes |
|---|---|---|
| 1 | `VMM`, `M2V`, `MergeV` | trivial |
| 2 | `BPF`, `AmplitudeBPF` | 2nd-order + rectify + envelope LPF |
| 3 | `VMMWTA` | train with softmax surrogate, eval with hard WTA |
| 4 | `LadderFM` | documented delay-line simplification if the ladder paper is not in hand |

**Money artifact:** `examples/acoustic_case1.py`

WAV → 16-band exp-spaced AmplitudeBPF (50 Hz–10 kHz) → LadderFM delays → M2V →
VMMWTA, weights trained offline on a 4-word Speech Commands subset, accuracy on
held-out. This *is* Fig 16's sample acoustic program.

**Gate B:** ship whatever accuracy you get, with the number of test utterances
and a CI if cheap. The deliverable is the executable suite, not 90%. If training
stalls, report chance-level honestly.

**Optional if Case I is green before hour 16:** LadderF sweep → ripple/stopband
on a synthetic chirp. Fully specified in TRETS §3.4, no dataset.

---

## Deliverables (this window)

1. `analog-benchmarks/ams-bench/` — `amsblocks/` + `analysis/` + `examples/acoustic_case1.py`
2. `FINDINGS_ams.md` — dead artifact link, any Fig 17–18 mismatches, LadderFM simplification
3. `report/Analog-Benchmarks-Reproduction.md` — ~1–2 pages: claims, what re-derived, Case I number
4. **One paragraph** appended to `hls-eval/email_draft.md` (trajectory + dead link + AMS-Eval as proposed first project). Not a second email.

## Stop-rules

1. No eighth script. A problem that needs one gets dropped.
2. One interpretation per block, documented. Paper text is authority this window (VM skipped).
3. Hour 20 is a hard stop. Unfinished work → one future-work sentence.
4. Do not start Phase C. Do not start SmartPAF.
