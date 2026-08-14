# Analog system benchmarks (Hasler & Hao)

TRETS 2024 benchmark definitions plus the JLPEA 2023 analog-HLS predecessor
(acoustic VMM + winner-take-all). No FPAA, no proprietary EDA.

Combined write-up: [`../report/Hao-Lab-Reproductions.pdf`](../report/Hao-Lab-Reproductions.pdf).
What was checked: [`FINDINGS_ams.md`](FINDINGS_ams.md).

| Paper | Role |
|---|---|
| Hasler & Hao, TRETS 17:1, 2024, [10.1145/3625298](https://doi.org/10.1145/3625298) | CAB costs, scaling, Fig 16 programs |
| Ige, Yang, Yang, Hasler, Hao, JLPEA 13(4):58, 2023, [open access](https://www.mdpi.com/2079-9268/13/4/58) | Case I acoustic classifier (VMM+WTA) |

The artifact URL in TRETS footnote 1, `hasler.ece.gatech.edu/AnalogBenchmarks`,
was already 404 on the Wayback Machine (2023-11-19).

## Run

```bash
cd ams-bench
python -m pip install -r requirements.txt
python tests/test_amsblocks.py
python -m analysis.scaling
python examples/acoustic_case1.py
```

Case II WTA: 128 CABs / 98 SoC CABs = **1.31 chips** (paper: barely more than
one, fits in two). Case I macromodel: **70%** held-out on yes/no/up/down
(chance 25%). LadderFM is an integer delay, not a gm-C ladder.
