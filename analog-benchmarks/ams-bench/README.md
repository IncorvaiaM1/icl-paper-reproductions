# ams-bench

Executable TRETS Fig 16 acoustic Case I and a re-derivation of Figs 3, 11,
and 18. numpy/scipy. See [`../FINDINGS_ams.md`](../FINDINGS_ams.md) and
the combined PDF at [`../../report/Hao-Lab-Reproductions.pdf`](../../report/Hao-Lab-Reproductions.pdf).

```bash
python -m pip install -r requirements.txt
python tests/test_amsblocks.py
python -m analysis.scaling
python examples/acoustic_case1.py
```
