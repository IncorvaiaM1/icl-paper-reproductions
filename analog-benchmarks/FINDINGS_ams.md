# FINDINGS — analog benchmarks (Phase 0+)

Started 2026-08-14. Same genre as `hls-eval/FINDINGS.md`: facts, not spin.

---

## 0.1 Artifact URL was already dead at capture time

Live check 2026-08-14: `https://hasler.ece.gatech.edu/AnalogBenchmarks` → **404**.

Wayback CDX (`url=hasler.ece.gatech.edu/AnalogBenchmarks`, prefix match):

| original | timestamp | status |
|---|---|---|
| `http://hasler.ece.gatech.edu/AnalogBenchmarks` | 2023-11-19 08:46:35 | **301** |
| `https://hasler.ece.gatech.edu/AnalogBenchmarks` | 2023-11-19 09:06:18 | **404** |

TRETS accepted Sep 2023, published Jan 2024. Footnote 1 points at that URL. The only Wayback captures are a redirect and a 404, six weeks after acceptance. There is no archived copy of the benchmark files. The 404 is a confirmed finding, not a transient outage.

**Authority for this window:** TRETS Figs 15–18 + JLPEA acoustic classifier. No external spec of record.

## 0.2 FPAA VM — skipped (48h cut)

## 0.3 Companion papers — local

NSF Public Access (award 2212179):

| File | Source |
|---|---|
| `papers/hasler_hao_trets_2024.pdf` | https://par.nsf.gov/servlets/purl/10504737 |
| `papers/ige_hasler_hao_jlpea_2023.pdf` | https://par.nsf.gov/servlets/purl/10504736 |

## Adjacent code (not AnalogBenchmarks)

JLPEA first author has a public analog-HLS repo: [a-folabi/ashes](https://github.com/a-folabi/ashes) (ASHES — FPAA switch lists / GDS from Python or XCOS). That is the *synthesis* toolchain, not the missing benchmark suite. Do not clone it this window unless a block is uninterpretable from the PDFs. The VM skip still stands.

## 0.4 Dataset

TensorFlow mini Speech Commands (`yes/no/up/down`). Full Speech Commands v2 and TIMIT were not used. TIMIT is LDC-licensed.

---

## A. Scaling and CAB arithmetic

Commands: `python -m analysis.scaling` from `ams-bench/`. Figure: `ams-bench/figures/scaling_check.png`.

### A.1 Bandwidth (Fig 3a / Fig 11 lower)

Four anchors interpolate exactly, by construction: 350 nm → 60 MHz, 130 nm → 500 MHz, 40 nm → 6 GHz, 14 nm → 50 GHz. Log-log linear between them. Below 14 nm the interpolant saturates at 50 GHz (no fifth anchor) — do not read that as a paper claim.

### A.2 / A.3 Case II acoustic CAB count (Fig 17–18)

Paper: 48-in × 128-out VMM+WTA, 6,144 weights; more WTA stages than fit on one SoC FPAA (barely); two 350 nm SoC FPAAs; ~250 µm × 300 µm in 130 nm std cells.

| Piece | CABs | How |
|---|---|---|
| AmplitudeBPF × 12 | 12 | Fig 18: 1 CAB each |
| LadderFM, 12 ch × 3 stages | 36 | Fig 18 LadderF rate (1 CAB/stage); LadderFM is a pointer to [75] |
| VMMWTA 48→128 | 128 | 1 out/CAB on SoC FPAA (body, p. 12:14) |
| **Total** | **176** | |
| WTA only | 128 | the sentence the paper actually writes |

SoC FPAA CAB count is **not printed in TRETS**. We use **98** from the SoC FPAA paper Fig 18 is baselined to (TRETS ref [31]). Then:

- WTA: 128 / 98 = **1.31 chips** — >1 and <2. The "barely > 1, fits in 2" claim **holds** for WTA-only.
- Full Fig 17 program: 176 / 98 = **1.80 chips** — still fits in two. If someone assumed ~128 CABs on the SoC, WTA would be exactly one chip and "barely" would not hold.

12 bands in the worked example, not 16–24. Body text elsewhere says "16 to 32 bands → 16 to 32 CABs" for the front-end; Fig 17 is a 12-line AmplitudeBPF. We cost the figure, not the prose.

130 nm area claim (250 µm × 300 µm) is a standard-cell number, not a CAB count. Not re-derived.

### A.4 Comm Case III

Paper: 2 GHz already exceeded by a 2×3 mm 40 nm FPAA [53]. Model bandwidth at 40 nm is the Fig 3a anchor, **6 GHz**. 2 GHz is met with 3× margin. Consistent; we did not re-read [53].

### Fig 11 CAB density

CABs vs node is **1/λ²** from 98 CABs @ 350 nm, 25 mm², times die area. That is the physically obvious model, not a pixel digitization of Fig 11. If the drawn lines are shallower than λ⁻², that is a finding for a later overlay — not fitted away.

---

## B. Executable Fig 16 (acoustic Case I)

`python examples/acoustic_case1.py`

Program (Fig 16, `freq=100`; Fig 17 Case II writes `freq=400` — we follow Fig 16 for Case I):

```
y1 = AmplitudeBPF(In, center=f1, Q=Q1, fpeak=f2)
y2 = M2V(LadderFM(input=y1, Ibias1=Ib1, Ibias2=Ib2))
Out = VMMWTA(input=y2, W=W1, Thresh=theta1, freq=100)
```

Sized as JLPEA Fig 3: 16 log-spaced bands (50 Hz–4 kHz, Nyquist-limited vs the paper's 10 kHz), 3 ladder stages, 4-word VMMWTA. Case I CAB cost **68.9** (fits one 98-CAB SoC FPAA), consistent with "Case I already demonstrated on the SoC FPAA."

**LadderFM simplification:** integer sample delay, 25 ms/stage. `Ibias1`/`Ibias2` accepted and ignored. Not a gm-C ladder.

**Classifier:** softmax train, hard-WTA eval. Features = Fig 16's `y2` pooled into 4 time bins (a clip-wide mean erases the delays).

Held-out, mini Speech Commands yes/no/up/down, n=200 test / 600 train:

**accuracy 0.70** (chance 0.25; ~95% interval [0.64, 0.76]). Per-class: yes 0.78, no 0.57, up 0.89, down 0.53.

Not 90%. The paper's silicon number is on TIMIT/TI-digits with analog dynamics we did not model. The deliverable is the executable program, not the accuracy.

Tests: `python tests/test_amsblocks.py` — 8 passed, including Case II 6144-weight / 1.31-chip arithmetic.
