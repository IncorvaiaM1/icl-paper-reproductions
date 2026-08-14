# Paper Notes — Hasler & Hao, "Programmable Analog System Benchmarks Leading to Efficient Analog Computation Synthesis" (TRETS 17:1, Art. 12, Jan 2024, DOI 10.1145/3625298)

_Why this paper matters for the Hao's-lab pitch: it is Callie's bridge paper into
analog — she brings the digital HLS/benchmark methodology (CHStone, MachSuite,
Rosetta lineage she cites) and Hasler brings 25 years of FPAA hardware. It is the
analog mirror of HLS-Eval: define benchmarks → enable synthesis-tool evaluation →
enable architectural DSE. Your HLS-Eval reproduction skills map onto it almost 1:1._

---

## 1. What the paper is

**Claim: the first representative analog & mixed-signal benchmark suite**, intended
to do for analog synthesis tools what LINPACK/CHStone/MachSuite/Rosetta did for
digital HPC/HLS. CC-BY, received Feb 2023, accepted Sep 2023.

Core argument: digital benchmarks reduce to `Ax = b` (high starting precision,
moderate summation); analog computation's native strengths are ODEs, PDEs,
continuous-time integration, and VMM (good numerics, moderate starting precision).
So analog benchmarks must be built around ODE/VMM-style computation, end-to-end
from sensor to classified output — not around isolated circuit blocks (they
explicitly exclude PLLs, level-shifters, data converters as "not benchmarks").

Target substrate: large-scale **floating-gate (FG) SoC FPAAs** (their 350 nm CMOS
chip: 600k programmable parameters, open MSP430 µP, CABs + CLBs, ~1,000×
energy advantage over digital per Mead's hypothesis; ≈1 TMAC/s effective, 10
GMAC/s in 1 mW). Scaling projections (Figs 3, 11): 40 nm ⇒ GHz-class fabric,
14 nm ⇒ 50–100 GHz fabric, "embedded supercomputing in mW."

## 2. The four benchmarks (Fig 7), each in four sizes (Case I–IV)

| Benchmark | Task | Case I → IV scaling | Datasets |
|---|---|---|---|
| **Acoustic** | mic → keyword/word classification, 20 kHz input | 4 → 32 → 128 → 1,000 words | TIMIT / TI-digits, Google Speech Commands, LibriSpeech |
| **Vision** | CMOS imager → image (I–II) then motion/video (III–IV) classification, 60 fps | MNIST → MNIST → 1 Mpixel → 10 Mpixel | MNIST, COCO 2017, SVD video |
| **Comm** | antenna array → beamforming + demodulation + symbol classification | 8/3 MHz → 16/50 MHz → 32/2 GHz → 64/20 GHz (inputs/center freq) | limited labeled RF sets (cite [85]) |
| **Filter** | 10th-order programmable low-pass filter, <2% ripple, ≥40 dB stopband at 1.2× passband | 300 kHz → 3 MHz → 30 MHz → 300 MHz | sinusoid sweep; metrics from output spectrum |

Rules of the game: all four must be satisfied *simultaneously* by a platform/tool
(anti-overfitting to one problem). Metrics: **energy/power, classification accuracy
measured from the sensor input, and area/resource utilization**; the vision
benchmark adds non-local communication + intermediate storage as a metric. Cases
map to CMOS nodes: 350 nm SoC FPAA ⇒ Case I (partially II); 130 nm ⇒ II; 40 nm ⇒
III/IV; 14 nm ⇒ Case IV comm (20 GHz). Training may be off-line or on-chip (on-chip
learning already demoed on FPAA); either way it must be reported with energy.

## 3. The abstraction & synthesis story (Secs 4–5)

- **level = 1**: vectorized, voltage-mode dataflow blocks (macromodeled, digital-simulatable). **level = 2**: full current–voltage analog cell design. Algorithm library (Fig 15): LPF, HopfLPF, LadderF/LadderFM, BPF, AdaptBPF, AmplitudeBPF, VMM, **VMMWTA**, SigVMM, VMMDemod, LinearEqIterate, NeuronHH (Hodgkin–Huxley), LocalSepImagerConv, ClassifyBlockImage2Vector, Scanner, M2V, MergeV — each with system params (freq, W, Q, centerFreq) lowered to circuit params (Ibias vectors).
- Each benchmark has a ~3-line sample "program" (Fig 16), e.g. acoustic:
  `y1 = AmplitudeBPF(In, center=f1, Q=Q1); y2 = M2V(LadderFM(y1, Ibias1, Ibias2)); Out = VMMWTA(y2, W=W1, Thresh=theta1, freq=400)`.
- Flow (Fig 14/17/19): Python (or Xcos graphical) → Verilog-AMS → netlist → either
  **switch list for FPAA** (VPR-based place & route, x2swc) or **.gds via FG-enabled
  programmable analog standard cells** (SkyWater 130 nm open-source std cell lib,
  ref [63]). Fig 18 gives CAB-cost per block on the SoC FPAA (e.g., BPF = 1/2 CAB,
  VMM = routing + 4 TA; the acoustic Case II example: 48-in × 128-out VMM+WTA with a
  6,144-weight matrix ≈ two 350 nm SoC FPAAs, or ~250 µm × 300 µm in 130 nm std cells).
- Future benchmark directions flagged: nonlinear dynamics / Hopfield / chaotic
  systems, PDEs (diffusion, wave), neuromorphic beyond rate-coded LIF, arbitrary
  waveform generation + modulation.

## 4. Reproducibility audit

**The headline gap: the artifact link is dead.** Footnote 1 says "All designs,
files, and reported numbers using these benchmarks are openly available" at
`https://hasler.ece.gatech.edu/AnalogBenchmarks` — that URL returns **404**
(checked 2026-08-13). The ICE lab homepage has no benchmarks link. Same genus as
HLS-Eval's placeholder `leaderboard.json`: benchmark infrastructure announced,
community uptake path missing.

**Also deferred at publication:** footnote 2 — "multiple articles currently in
submission" for the synthesis section; footnote 3 — "the synthesis flow to an FPAA
or custom layout from these Python definitions is currently under review." The
likely successor is Hasler's MDPI JLPEA 2023 paper "Analog System High-Level
Synthesis for Energy-Efficient Reconfigurable Computing" — worth reading next.

**Reproducible today, no hardware, no license:**
- All acoustic/vision datasets are public (TIMIT via LDC/github mirror, TI-digits,
  Speech Commands, LibriSpeech, MNIST, COCO, SVD).
- The level=1 block semantics (Fig 15 signatures + Fig 16 programs) are specified
  well enough to re-implement as Python macromodels; the paper itself says every
  block "should have a macromodel for potential digital simulation" and that the
  benchmarks "could also be solved using digital computation."
- The scaling extrapolations (Figs 3, 11) and CAB-count arithmetic (Fig 18 → the
  Case II acoustic sizing) can be re-derived and checked.
- Digital baselines (MCU/FPGA implementations of e.g. acoustic Case I/II) for the
  energy/accuracy comparison the paper motivates but never tabulates.

**Reproducible with the lab's free tooling (no fab):** the ICE lab distributes an
Ubuntu VM with the Scilab/Xcos → switch-list FPAA tool flow and FPAA board design
files (hasler.ece.gatech.edu/FPAAtool). With a RASP 3.0 / SoC FPAA board (or the
lab's remote-test setup) you can run Case I acoustic on real hardware — Case I has
already been demonstrated on the SoC FPAA (refs [31, 73–75, 80]).

**Not reproducible outside the lab:** measured energy numbers on their silicon;
the Python→Verilog-AMS→gds synthesis path (unreleased at pub time); scaled-node
(40/14 nm) results, which are projections, not measurements.

## 5. Novel next steps (ranked by fit to you + the labs)

1. **"AMS-Eval": an LLM benchmark for analog algorithm synthesis.** The exact
   HLS-Eval recipe applied to this paper's abstraction: prompt an LLM with a
   benchmark spec (e.g., acoustic Case I) + the Fig 15 block library, ask it to
   emit a level=1 Python program; grade with a metric ladder that mirrors
   parseable → compilable → runnable → synthesizable as **parseable → macromodel-
   simulatable → functionally-correct-on-dataset → within-CAB-budget (Fig 18
   costs)**. Nobody has done LLM-eval for analog system synthesis; both authors
   have obvious stakes in it, and you'd arrive with the only working harness
   pattern. This is the flagship pitch.
2. **Ship the missing reference macromodel library.** A small Python package
   implementing the Fig 15 blocks (BPF filterbank, LadderFM delays, VMM+WTA, etc.)
   that runs the four Fig 16 sample programs end-to-end on the public datasets.
   It is simultaneously (a) the executable spec the dead link was supposed to be,
   (b) the grading backend for step 1, and (c) the analog analogue of your
   `stock_tools.py` "three metrics without Vitis" move.
3. **Report the dead artifact link + offer the fix.** One sentence in the email;
   offer to stand up the benchmark repo/leaderboard the way you populated
   HLS-Eval's. Low effort, high credibility — it shows you read *her other*
   benchmark paper and found the same infrastructure gap.
4. **Digital baseline row.** Run acoustic Case I/II as a conventional INT8
   Keyword-spotting pipeline on an MCU/FPGA, report accuracy + energy, giving the
   suite its first populated cross-technology comparison — the thing benchmarks
   exist for and which the paper leaves as future work.
5. **Automated DSE (longer-term / thesis-scale).** The paper's stated end-goal is
   architectural design-space exploration to derive "the best FPAA for a family of
   applications." Sharc lab does GNN/ML-driven DSE for FPGAs; casting FPAA CAB/CLB
   sizing + routing as a learned DSE problem over these benchmarks is an
   unclaimed, well-posed research program.
6. **Hardware validation via the FPAA VM/remote boards** — only if the
   relationship advances; needs their hardware in the loop.

## 6. How this feeds the email

The email currently proves you can reproduce *her* digital benchmark. A single
added sentence can prove trajectory, e.g.: having read the TRETS analog benchmark
paper, you noticed the AnalogBenchmarks artifact URL 404s, and the level=1 block
programs (Fig 16) would support an HLS-Eval-style LLM evaluation — parse /
macromodel-simulate / dataset-accuracy / CAB-budget — which you'd be interested in
building as a first project. That connects the reproduction (evidence) to a
research direction (intent) that sits precisely at the Hasler↔Hao intersection.

---
_Sources: the uploaded TRETS PDF (10.1145/3625298); hasler.ece.gatech.edu (ICE lab
homepage, FPAA tools); dead-link check of /AnalogBenchmarks on 2026-08-13; MDPI
JLPEA 13(4):58 (follow-up synthesis paper, via search)._
