# Analog system benchmarks (Hasler & Hao)

Two papers, one project. The TRETS 2024 paper defines the analog/mixed-signal
benchmark suite. The JLPEA 2023 paper is the direct predecessor: same FPAA
analog-HLS toolchain, with a worked acoustic classifier (VMM + winner-take-all)
that is the architecture to target.

| Paper | Venue | Role here |
|---|---|---|
| Hasler & Hao, *Programmable Analog System Benchmarks…* | TRETS 17:1, 2024, [10.1145/3625298](https://doi.org/10.1145/3625298) | Benchmark definitions, CAB costs, scaling curves, Fig 16 programs |
| Ige, Yang, Yang, Hasler, Hao, *Analog System HLS for Energy-Efficient Reconfigurable Computing* | JLPEA 13(4):58, 2023, [open access](https://www.mdpi.com/2079-9268/13/4/58) | Acoustic VMM+WTA classifier spec for Case I |

## What "reproduce" means

There is no pass@k table. The claims that can be checked without an FPAA board
are arithmetic (Figs 3, 11, 17–18) plus making the Fig 16 block programs
executable on public datasets. The lab's artifact URL
`https://hasler.ece.gatech.edu/AnalogBenchmarks` 404s (checked 2026-08-13).

## Plans

- **This window:** [`GAMEPLAN_48h.md`](GAMEPLAN_48h.md) — Phase 0 → A → acoustic Case I. Stop there.
- **Stretch (out of scope):** [`GAMEPLAN.md`](GAMEPLAN.md) — full `amsblocks` library + AMS-Eval LLM harness. Mention as proposed first project; do not build it in the next couple of days.

## Local papers

- [`papers/hasler_hao_trets_2024.pdf`](papers/hasler_hao_trets_2024.pdf) — NSF PAR 10504737
- [`papers/ige_hasler_hao_jlpea_2023.pdf`](papers/ige_hasler_hao_jlpea_2023.pdf) — NSF PAR 10504736

## Code (48h cut)

[`ams-bench/`](ams-bench/) — `python tests/test_amsblocks.py`, `python -m analysis.scaling`, `python examples/acoustic_case1.py`

Case II WTA: 128 CABs / 98 = 1.31 SoC FPAAs (claim holds). Case I digital macromodel: **70%** on yes/no/up/down (chance 25%). Details in [`FINDINGS_ams.md`](FINDINGS_ams.md).

## Notes

- [`paper_notes_trets.md`](paper_notes_trets.md) — TRETS paper, reproducibility audit, novel next steps
- [`paper_notes_jlpea.md`](paper_notes_jlpea.md) — JLPEA analog-HLS paper, acoustic classifier motif
- [`FINDINGS_ams.md`](FINDINGS_ams.md) — dead artifact URL, CAB arithmetic, Case I accuracy
