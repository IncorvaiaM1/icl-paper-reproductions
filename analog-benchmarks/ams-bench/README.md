# ams-bench — TRETS 2024 + JLPEA 2023, 48-hour cut

Executable Fig 16 acoustic Case I and a re-derivation of the Fig 3 / Fig 11
scaling curves plus the Fig 18 CAB table. numpy/scipy only. No FPAA, no Vitis.

```
python -m pip install -r requirements.txt
python tests/test_amsblocks.py
python -m analysis.scaling
python examples/acoustic_case1.py
```

Run the last three from this directory (`ams-bench/`). `acoustic_case1.py`
downloads TensorFlow's mini Speech Commands zip on first run (~few MB) and
keeps yes/no/up/down.

Fig 16 program as executed:

```
y1 = AmplitudeBPF(In, center=f1, Q=Q1, fpeak=f2)
y2 = M2V(LadderFM(input=y1, Ibias1=Ib1, Ibias2=Ib2))
Out = VMMWTA(input=y2, W=W1, Thresh=theta1, freq=100)
```

LadderFM is a documented integer-delay stand-in for the gm-C ladder in [75].
See `../FINDINGS_ams.md`.
