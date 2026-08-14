"""Unit tests for Fig 18 costs and Fig 16 block semantics. No dataset, no network."""

from __future__ import annotations

import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from amsblocks.blocks import AmplitudeBPF, BPF, LadderFM, M2V, VMM, VMMWTA, exp_spaced_centers
from amsblocks.costs import SOC_FPAA_CABS, cab_cost, check_case2


def test_case2_weights_and_wta_chips() -> None:
    r = check_case2()
    assert r["weights"] == 6144
    assert r["wta_cabs"] == 128  # 1 out/CAB × 128
    assert r["wta_vs_one_chip"]["cabs_per_chip"] == SOC_FPAA_CABS
    assert r["wta_claim_holds"], r["wta_vs_one_chip"]
    assert r["full_claim_holds"], r["full_program_vs_one_chip"]


def test_case1_cab_fits_one_soc() -> None:
    from amsblocks.costs import case1_acoustic_program, chips_needed

    bd = cab_cost(case1_acoustic_program(16, 3, 4))
    chips = chips_needed(bd.cabs)
    assert chips["chips"] < 1.0, chips


def test_bpf_peaks_near_center() -> None:
    fs = 16000.0
    t = np.arange(fs) / fs
    f0 = 1000.0
    x = np.sin(2 * np.pi * f0 * t)
    y = BPF(x, center=[500.0, 1000.0, 2000.0], Q=[4.0, 4.0, 4.0], fs=fs)
    rms = np.sqrt((y**2).mean(axis=0))
    assert int(np.argmax(rms)) == 1


def test_ladder_delay_shift() -> None:
    fs = 1000.0
    x = np.zeros(100)
    x[10] = 1.0
    y = LadderFM(x, n_stages=2, delay_s=0.01, fs=fs)  # 10 samples/stage
    assert y.shape == (100, 3, 1)
    assert np.argmax(y[:, 0, 0]) == 10
    assert np.argmax(y[:, 1, 0]) == 20
    assert np.argmax(y[:, 2, 0]) == 30


def test_m2v_flattens() -> None:
    x = np.arange(24).reshape(2, 3, 4)
    v = M2V(x)
    assert v.shape == (2, 12)
    assert v[0, 0] == 0 and v[0, -1] == 11


def test_vmmwta_learns_separable() -> None:
    rng = np.random.default_rng(1)
    Wtrue = rng.normal(size=(8, 3))
    X = rng.normal(size=(200, 8))
    y = np.argmax(X @ Wtrue, axis=1)
    clf = VMMWTA.zeros(8, 3)
    clf.fit(X, y, lr=0.3, epochs=60, rng=rng)
    pred = np.argmax(clf.logits(X), axis=1)
    assert np.mean(pred == y) > 0.85


def test_exp_centers() -> None:
    c = exp_spaced_centers(16, 50, 10_000)
    assert c[0] == 50
    assert np.isclose(c[-1], 10_000)
    assert np.all(np.diff(np.log(c)) > 0)


def test_amplitude_bpf_shape() -> None:
    fs = 8000.0
    x = np.random.default_rng(0).normal(size=800)
    y = AmplitudeBPF(x, center=exp_spaced_centers(4, 100, 2000), Q=3.0, fs=fs)
    assert y.shape == (800, 4)
    assert np.all(y >= -1e-12)  # envelope is non-negative


if __name__ == "__main__":
    tests = [v for k, v in list(globals().items()) if k.startswith("test_")]
    failed = 0
    for fn in tests:
        try:
            fn()
            print(f"ok  {fn.__name__}")
        except Exception as e:
            failed += 1
            print(f"FAIL {fn.__name__}: {e!r}")
    if failed:
        raise SystemExit(1)
    print(f"{len(tests)} passed")
