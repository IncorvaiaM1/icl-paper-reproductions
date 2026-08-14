# Paper Notes — Ige, Yang, Yang, Hasler, Hao, "Analog System High-Level Synthesis for Energy-Efficient Reconfigurable Computing" (JLPEA 13(4):58, 2023)

Open access: https://www.mdpi.com/2079-9268/13/4/58

_Folded into the TRETS analog-benchmarks project. Not a separate reproduction._

## What it is

Direct predecessor to Hasler & Hao TRETS 2024. Same FPAA analog-HLS toolchain
(Python framework + XCOS GUI → GDSII or FPAA switch list). Flagship demo is an
**acoustic classifier**: small-vocabulary (2–5 word) command classification via
a "universal approximator classifier" — analog VMM (vector-matrix multiply) +
winner-take-all. That is the analog/neuromorphic NN primitive, not a generic
filter.

## Why it matters for the 48h slice

Stronger ground-truth for `examples/acoustic_case1.py` than TRETS alone. TRETS
gives the Fig 16 program and CAB table; this paper gives the actual VMM+WTA
architecture to target in `amsblocks`.

## Reproducibility

MEDIUM-LOW as a strict repro (no AnalogBenchmarks artifact). HIGH as a spec to
build against. First author later open-sourced the *synthesis* tool as
[a-folabi/ashes](https://github.com/a-folabi/ashes) — that is ASHES (Python/XCOS
→ FPAA switch list / GDS), not the missing benchmark suite. Do not clone it
this window unless a block is uninterpretable from the PDF.

## What to pull from it during Phase 0.3

- Filterbank band count / spacing used in the acoustic demo
- VMM+WTA topology (inputs, WTA outputs, how winner is taken)
- Any reported classification accuracy on the 2–5 word task (this becomes the
  honesty baseline: "paper reported X on FPAA silicon; we got Y on a digital
  macromodel")
