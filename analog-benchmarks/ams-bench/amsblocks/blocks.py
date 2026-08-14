"""Tier-F (discrete-time, deterministic) macromodels of TRETS Fig 15 blocks
needed for Fig 16 acoustic Case I.

Authority this window: TRETS Fig 15 signatures + Fig 16/17 programs +
JLPEA 2023 Fig 3 (16-band C4 BPF + envelope + ladder delay + VMM+WTA).
No FPAA VM — LadderFM is an explicit delay-line stand-in, documented below.
"""

from __future__ import annotations

from dataclasses import dataclass

import numpy as np
from numpy.typing import NDArray
from scipy.signal import lfilter


Array = NDArray[np.floating]


def M2V(x: Array) -> Array:
    """Matrix → vector. Flattens the last two axes, row-major (C order).

    Fig 15: input m×n, output m·n. Used after LadderFM so VMMWTA sees a vector.
    """
    x = np.asarray(x, dtype=float)
    if x.ndim == 1:
        return x
    return np.reshape(x, (*x.shape[:-2], -1))


def MergeV(a: Array, b: Array) -> Array:
    """Concatenate two vectors. Fig 15: (m, n) → m+n."""
    return np.concatenate([np.asarray(a, dtype=float), np.asarray(b, dtype=float)], axis=-1)


def VMM(x: Array, W: Array) -> Array:
    """y = x @ W. Fig 15: input m, output n, W is m×n (rows = inputs).

    TRETS notes a bare VMM is current-out and therefore not level=1; the
    level=1 forms are VMMWTA / SigVMM. This is the linear core they share.
    """
    return np.asarray(x, dtype=float) @ np.asarray(W, dtype=float)


def _rbj_bandpass(f0: float, Q: float, fs: float) -> tuple[Array, Array]:
    """RBJ audio EQ cookbook bandpass (constant 0 dB peak gain)."""
    w0 = 2.0 * np.pi * f0 / fs
    alpha = np.sin(w0) / (2.0 * Q)
    b0 = alpha
    b1 = 0.0
    b2 = -alpha
    a0 = 1.0 + alpha
    a1 = -2.0 * np.cos(w0)
    a2 = 1.0 - alpha
    b = np.array([b0 / a0, b1 / a0, b2 / a0], dtype=float)
    a = np.array([1.0, a1 / a0, a2 / a0], dtype=float)
    return b, a


def BPF(x: Array, center: Array, Q: Array, fs: float) -> Array:
    """Parallel 2nd-order bandpass bank. Fig 15: input 1, output n.

    Each channel is an RBJ biquad at ``center[i]`` Hz with quality ``Q[i]``.
    This is the digital stand-in for the C4 (gm-C) BPF in JLPEA Fig 3.
    """
    x = np.asarray(x, dtype=float)
    center = np.atleast_1d(np.asarray(center, dtype=float))
    Q = np.atleast_1d(np.asarray(Q, dtype=float))
    if Q.size == 1:
        Q = np.full(center.shape, float(Q[0]))
    out = np.empty((x.shape[0], center.size), dtype=float)
    for i, (f0, q) in enumerate(zip(center, Q)):
        b, a = _rbj_bandpass(float(f0), float(q), fs)
        out[:, i] = lfilter(b, a, x)
    return out


def _onepole_lpf(x: Array, fc: float, fs: float) -> Array:
    x = np.asarray(x, dtype=float)
    w = 2.0 * np.pi * fc / fs
    a = np.exp(-w)
    b = 1.0 - a
    return lfilter([b], [1.0, -a], x, axis=0)


def AmplitudeBPF(
    x: Array,
    center: Array,
    Q: Array,
    fs: float,
    fpeak: float | None = None,
) -> Array:
    """BPF + rectify + envelope LPF. Fig 15 / Fig 16 acoustic line 1.

    ``fpeak`` (Fig 16) is the envelope detector cutoff. Default 50 Hz — a
    typical speech amplitude-detect bias, used when the paper leaves it as
    a parameter vector rather than a number.
    """
    y = BPF(x, center, Q, fs)
    env_cut = 50.0 if fpeak is None else float(fpeak)
    return _onepole_lpf(np.abs(y), env_cut, fs)


def LadderFM(
    x: Array,
    n_stages: int,
    delay_s: float,
    fs: float,
    Ibias1: Array | None = None,
    Ibias2: Array | None = None,
) -> Array:
    """Multi-channel tapped delay. Fig 15: input m, output n×m.

    **Simplification (no VM / no [75] Xcos code this window):** a gm-C
    ladder delay is replaced by an integer sample delay of ``delay_s``
    seconds per stage. ``Ibias1`` / ``Ibias2`` are accepted for signature
    compatibility with Fig 16 and ignored; delay is the system parameter
    we actually use. Output shape is ``(T, n_stages+1, m)`` — tap 0 is the
    undelayed input so Case II's 12×4 = 48 lines fall out of 12 bands and
    3 stages.
    """
    x = np.asarray(x, dtype=float)
    if x.ndim == 1:
        x = x[:, None]
    t_len, m = x.shape
    d = max(1, int(round(delay_s * fs)))
    taps = np.zeros((t_len, n_stages + 1, m), dtype=float)
    taps[:, 0, :] = x
    for k in range(1, n_stages + 1):
        shift = k * d
        if shift < t_len:
            taps[shift:, k, :] = x[: t_len - shift]
    return taps


def _softmax(z: Array) -> Array:
    z = np.asarray(z, dtype=float)
    z = z - np.max(z, axis=-1, keepdims=True)
    e = np.exp(z)
    return e / np.sum(e, axis=-1, keepdims=True)


@dataclass
class VMMWTA:
    """k-WTA on top of a VMM. Fig 15: input m, output n, params W, Thresh, freq.

    Train with a softmax surrogate (digital stand-in for analog competitive
    dynamics). Evaluate with hard WTA (argmax). ``Thresh`` is subtracted
    from the VMM output before competition, matching the programmable WTA
    bias in JLPEA Fig 3. ``freq`` is accepted and unused at tier-F — it
    exists so tools can lower W → Ibias (TRETS p. 12:16).
    """

    W: Array
    Thresh: Array
    freq: float = 100.0

    @classmethod
    def zeros(cls, n_in: int, n_out: int, freq: float = 100.0) -> "VMMWTA":
        return cls(W=np.zeros((n_in, n_out)), Thresh=np.zeros(n_out), freq=freq)

    def logits(self, x: Array) -> Array:
        return VMM(x, self.W) - self.Thresh

    def __call__(self, x: Array) -> Array:
        z = self.logits(x)
        winner = np.argmax(z, axis=-1)
        out = np.zeros_like(z)
        if z.ndim == 1:
            out[winner] = 1.0
        else:
            out[np.arange(z.shape[0]), winner] = 1.0
        return out

    def fit(
        self,
        X: Array,
        y: Array,
        lr: float = 0.05,
        epochs: int = 80,
        l2: float = 1e-4,
        rng: np.random.Generator | None = None,
    ) -> list[float]:
        """Softmax / cross-entropy on utterance-level feature rows."""
        rng = rng or np.random.default_rng(0)
        X = np.asarray(X, dtype=float)
        y = np.asarray(y, dtype=int)
        n_out = self.W.shape[1]
        Y = np.eye(n_out)[y]
        losses: list[float] = []
        for _ in range(epochs):
            idx = rng.permutation(X.shape[0])
            Xb, Yb = X[idx], Y[idx]
            p = _softmax(self.logits(Xb))
            err = p - Yb
            self.W -= lr * (Xb.T @ err / X.shape[0] + l2 * self.W)
            self.Thresh -= lr * (-err.mean(axis=0))
            losses.append(float(-np.mean(np.sum(Yb * np.log(p + 1e-12), axis=1))))
        return losses


def exp_spaced_centers(n: int, fmin: float = 50.0, fmax: float = 10_000.0) -> Array:
    """JLPEA / TRETS acoustic front-end: n bands, 50 Hz–10 kHz, log-spaced."""
    return np.geomspace(fmin, fmax, n)
