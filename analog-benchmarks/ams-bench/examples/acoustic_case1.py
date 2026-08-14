"""Fig 16 acoustic Case I: WAV → AmplitudeBPF → LadderFM → M2V → VMMWTA.

Matches TRETS Fig 16's three-line program, sized as JLPEA Fig 3 Case I
(16 subbands, 2–5 word vocabulary). Dataset: TensorFlow mini Speech
Commands, labels yes/no/up/down. TIMIT is LDC-licensed and is not used.
"""

from __future__ import annotations

import io
import sys
import zipfile
from pathlib import Path
from urllib.request import urlopen

import numpy as np
from scipy.io import wavfile

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT))

from amsblocks.blocks import (  # noqa: E402
    AmplitudeBPF,
    LadderFM,
    M2V,
    VMMWTA,
    exp_spaced_centers,
)
from amsblocks.costs import cab_cost, case1_acoustic_program  # noqa: E402

MINI_URL = "https://storage.googleapis.com/download.tensorflow.org/data/mini_speech_commands.zip"
LABELS = ("yes", "no", "up", "down")
N_BANDS = 16
N_STAGES = 3
Q = 4.0
DELAY_S = 0.025  # 25 ms per ladder stage — speech-scale, not from a measured Ibias
FMIN, FMAX = 50.0, 4000.0  # clips are 16 kHz; cap below Nyquist
TARGET_FS = 16000
DATA_DIR = ROOT / "data" / "mini_speech_commands"


def _ensure_dataset() -> Path:
    if all((DATA_DIR / lab).is_dir() for lab in LABELS):
        return DATA_DIR
    DATA_DIR.parent.mkdir(parents=True, exist_ok=True)
    print(f"downloading {MINI_URL}")
    with urlopen(MINI_URL, timeout=120) as resp:
        blob = resp.read()
    with zipfile.ZipFile(io.BytesIO(blob)) as zf:
        zf.extractall(DATA_DIR.parent)
    # archive extracts to mini_speech_commands/
    if not DATA_DIR.is_dir():
        raise FileNotFoundError(f"expected {DATA_DIR} after extract")
    return DATA_DIR


def _load_wav(path: Path) -> np.ndarray:
    fs, x = wavfile.read(path)
    x = np.asarray(x, dtype=float)
    if x.ndim > 1:
        x = x.mean(axis=1)
    x = x / (np.max(np.abs(x)) + 1e-12)
    if fs != TARGET_FS:
        # integer-ratio resample only; mini-speech is 16 kHz
        if fs % TARGET_FS == 0:
            x = x[:: fs // TARGET_FS]
        elif TARGET_FS % fs == 0:
            x = np.repeat(x, TARGET_FS // fs)
        else:
            n = int(round(len(x) * TARGET_FS / fs))
            t_old = np.linspace(0.0, 1.0, len(x), endpoint=False)
            t_new = np.linspace(0.0, 1.0, n, endpoint=False)
            x = np.interp(t_new, t_old, x)
    if len(x) < TARGET_FS:
        x = np.pad(x, (0, TARGET_FS - len(x)))
    return x[:TARGET_FS]


def fig16_features(wav: np.ndarray, fs: float, centers: np.ndarray, n_bins: int = 4) -> np.ndarray:
    """Execute Fig 16, then pool y2 into ``n_bins`` time bins.

    A single clip-wide mean would wash out the LadderFM delays. Four
    successive means keep spectro-temporal structure for the VMM.
    """
    y1 = AmplitudeBPF(wav, center=centers, Q=Q, fs=fs, fpeak=50.0)
    y2 = M2V(LadderFM(y1, n_stages=N_STAGES, delay_s=DELAY_S, fs=fs))
    t_len = y2.shape[0]
    edges = np.linspace(0, t_len, n_bins + 1, dtype=int)
    parts = [y2[edges[i] : edges[i + 1]].mean(axis=0) for i in range(n_bins)]
    return np.concatenate(parts)


def _gather(rng: np.random.Generator, max_per_class: int = 200) -> tuple[np.ndarray, np.ndarray]:
    _ensure_dataset()
    xs, ys = [], []
    centers = exp_spaced_centers(N_BANDS, FMIN, FMAX)
    for yi, lab in enumerate(LABELS):
        files = sorted((DATA_DIR / lab).glob("*.wav"))
        rng.shuffle(files)
        files = files[:max_per_class]
        for p in files:
            try:
                wav = _load_wav(p)
            except Exception:
                continue
            xs.append(fig16_features(wav, TARGET_FS, centers))
            ys.append(yi)
        print(f"  {lab}: {sum(1 for y in ys if y == yi)} utterances")
    return np.stack(xs), np.asarray(ys, dtype=int)


def _split(X, y, rng, frac=0.25):
    idx = rng.permutation(len(y))
    n_te = max(1, int(round(len(y) * frac)))
    te, tr = idx[:n_te], idx[n_te:]
    return X[tr], y[tr], X[te], y[te]


def main() -> None:
    rng = np.random.default_rng(0)
    print("Fig 16 acoustic Case I - yes/no/up/down via AmplitudeBPF + LadderFM + VMMWTA")
    prog = case1_acoustic_program(N_BANDS, N_STAGES, n_words=len(LABELS))
    cost = cab_cost(prog)
    print(f"CAB cost: {cost.cabs:.1f}  parts={cost.parts}")

    X, y = _gather(rng)
    Xtr, ytr, Xte, yte = _split(X, y, rng)
    # z-score on train
    mu, sd = Xtr.mean(0), Xtr.std(0) + 1e-6
    Xtr = (Xtr - mu) / sd
    Xte = (Xte - mu) / sd

    clf = VMMWTA.zeros(Xtr.shape[1], len(LABELS), freq=100.0)
    clf.fit(Xtr, ytr, lr=0.15, epochs=200, l2=1e-3, rng=rng)
    pred = np.argmax(clf.logits(Xte), axis=-1)
    acc = float(np.mean(pred == yte))
    chance = 1.0 / len(LABELS)
    n_te = len(yte)
    # Wilson-ish 95% interval via normal approx
    se = np.sqrt(acc * (1 - acc) / n_te)
    print(f"held-out accuracy: {acc:.3f}  (n={n_te}, chance={chance:.2f}, +/-1.96 se -> "
          f"[{acc - 1.96 * se:.3f}, {acc + 1.96 * se:.3f}])")
    for i, lab in enumerate(LABELS):
        m = yte == i
        if m.any():
            print(f"  {lab}: {np.mean(pred[m] == yte[m]):.3f}  (n={int(m.sum())})")

    out = ROOT / "results" / "acoustic_case1.json"
    out.parent.mkdir(parents=True, exist_ok=True)
    import json

    out.write_text(
        json.dumps(
            {
                "labels": list(LABELS),
                "n_bands": N_BANDS,
                "n_stages": N_STAGES,
                "n_train": int(len(ytr)),
                "n_test": int(n_te),
                "accuracy": acc,
                "chance": chance,
                "ci95": [acc - 1.96 * se, acc + 1.96 * se],
                "cab_cost": cost.as_dict(),
                "program": "y1=AmplitudeBPF(In,center=f1,Q=Q1,fpeak=f2); "
                "y2=M2V(LadderFM(y1,Ibias1,Ibias2)); "
                "Out=VMMWTA(y2,W=W1,Thresh=theta1,freq=100)",
            },
            indent=2,
        )
    )
    print(f"wrote {out}")


if __name__ == "__main__":
    main()
