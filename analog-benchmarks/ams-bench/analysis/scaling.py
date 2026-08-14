"""Fig 3 / Fig 11 scaling re-derivation.

Anchors are taken from the TRETS captions and body text, not digitized
pixels. Overlaying these closed-form curves on the paper's figures is the
check; slope disagreements are findings, not numbers we fit away.

Anchors (TRETS Fig 3 caption + body, Fig 11 caption)
----------------------------------------------------
Fabric bandwidth:  350 nm → 60 MHz, 130 nm → 500 MHz,
                   40 nm → 6 GHz,  14 nm → 50 GHz.
Die areas (Fig 11): 6 mm² (2×3), 25 mm² (5×5), 100 mm² (10×10).
FG count: 600,000 programmable parameters on the 350 nm SoC FPAA.
MAC/s: ≈1 TMAC/s on existing 350 nm FPAAs; 10 GMAC/s in 1 mW.
CABs: 98 on the 350 nm SoC FPAA (ref [31]); density scales as 1/λ².

Comm Case III (A.4): 2 GHz 'already exceeded by a 2 mm × 3 mm, 40 nm CMOS
FPAA [53]'. Our 40 nm bandwidth anchor is 6 GHz, so 2 GHz is met with
margin on the stated 6 mm² die — consistent.
"""

from __future__ import annotations

from pathlib import Path

import numpy as np

# (process node nm, fabric bandwidth Hz)
BANDWIDTH_ANCHORS = np.array(
    [
        (350.0, 60e6),
        (130.0, 500e6),
        (40.0, 6e9),
        (14.0, 50e9),
    ]
)

DIE_AREAS_MM2 = {"6mm2": 6.0, "25mm2": 25.0, "100mm2": 100.0}

SOC_NODE_NM = 350.0
SOC_FG = 600_000
SOC_CABS = 98
SOC_DIE_MM2 = 25.0  # Fig 3c is drawn for 5×5 mm² devices
SOC_TMAC_S = 1e12  # ≈1 TMAC/s existing 350 nm
SOC_GMAC_PER_MW = 10e9  # 10 GMAC/s in 1 mW

# Comm Case III
CASE_III_COMM_HZ = 2e9
CASE_III_DIE_MM2 = 6.0  # 2×3 mm
CASE_III_NODE_NM = 40.0


def fabric_bandwidth_hz(node_nm: np.ndarray | float) -> np.ndarray:
    """Log-log interpolation through the four Fig 3a anchors."""
    x = np.log10(np.asarray(node_nm, dtype=float))
    xp = np.log10(BANDWIDTH_ANCHORS[:, 0])
    yp = np.log10(BANDWIDTH_ANCHORS[:, 1])
    # np.interp needs increasing xp; node decreases with performance, so flip
    order = np.argsort(xp)
    y = 10 ** np.interp(x, xp[order], yp[order])
    return y


def density_scale(node_nm: np.ndarray | float) -> np.ndarray:
    """Ideal 1/λ² density relative to the 350 nm SoC FPAA."""
    return (SOC_NODE_NM / np.asarray(node_nm, dtype=float)) ** 2


def fg_count(node_nm: np.ndarray | float, area_mm2: float = SOC_DIE_MM2) -> np.ndarray:
    return SOC_FG * density_scale(node_nm) * (area_mm2 / SOC_DIE_MM2)


def cab_count(node_nm: np.ndarray | float, area_mm2: float) -> np.ndarray:
    return SOC_CABS * density_scale(node_nm) * (area_mm2 / SOC_DIE_MM2)


def mac_per_s(node_nm: np.ndarray | float, area_mm2: float = SOC_DIE_MM2) -> np.ndarray:
    """Scale the 1 TMAC/s @ 350 nm / 25 mm² caption by density and area."""
    return SOC_TMAC_S * density_scale(node_nm) * (area_mm2 / SOC_DIE_MM2)


def check_comm_case3() -> dict[str, float | bool]:
    bw = float(np.asarray(fabric_bandwidth_hz(CASE_III_NODE_NM)))
    return {
        "node_nm": CASE_III_NODE_NM,
        "die_mm2": CASE_III_DIE_MM2,
        "paper_metric_hz": CASE_III_COMM_HZ,
        "model_bandwidth_hz": bw,
        "met": bw >= CASE_III_COMM_HZ,
        "margin_x": bw / CASE_III_COMM_HZ,
    }


def plot_scaling(out_path: Path) -> Path:
    import matplotlib.pyplot as plt

    nodes = np.logspace(np.log10(10), np.log10(350), 80)
    fig, axes = plt.subplots(2, 2, figsize=(9.5, 7.5))

    ax = axes[0, 0]
    ax.loglog(nodes, fabric_bandwidth_hz(nodes) / 1e9, color="C0", lw=2)
    ax.scatter(
        BANDWIDTH_ANCHORS[:, 0],
        BANDWIDTH_ANCHORS[:, 1] / 1e9,
        color="C0",
        zorder=3,
        label="paper anchors",
    )
    ax.set_xlabel("process node (nm)")
    ax.set_ylabel("fabric bandwidth (GHz)")
    ax.set_title("Fig 3a / Fig 11 lower — fabric bandwidth")
    ax.legend(frameon=False)
    ax.invert_xaxis()

    ax = axes[0, 1]
    for name, area in DIE_AREAS_MM2.items():
        ax.loglog(nodes, cab_count(nodes, area), lw=2, label=name)
    ax.set_xlabel("process node (nm)")
    ax.set_ylabel("CABs (1/λ² from 98 @ 350 nm, 25 mm²)")
    ax.set_title("Fig 11 upper — CABs vs node and die area")
    ax.legend(frameon=False)
    ax.invert_xaxis()

    ax = axes[1, 0]
    ax.loglog(nodes, fg_count(nodes) / 1e6, color="C1", lw=2)
    ax.scatter([SOC_NODE_NM], [SOC_FG / 1e6], color="C1", zorder=3, label="600k @ 350 nm")
    ax.set_xlabel("process node (nm)")
    ax.set_ylabel("FG devices (millions), 25 mm²")
    ax.set_title("Fig 3b — FG count")
    ax.legend(frameon=False)
    ax.invert_xaxis()

    ax = axes[1, 1]
    ax.loglog(nodes, mac_per_s(nodes) / 1e12, color="C2", lw=2)
    ax.scatter([SOC_NODE_NM], [SOC_TMAC_S / 1e12], color="C2", zorder=3, label="1 TMAC/s @ 350 nm")
    ax.set_xlabel("process node (nm)")
    ax.set_ylabel("VMM compute (TMAC/s), 25 mm²")
    ax.set_title("Fig 3c — MAC/s")
    ax.legend(frameon=False)
    ax.invert_xaxis()

    fig.suptitle("TRETS 2024 Figs 3 & 11 re-derived from stated anchors", fontsize=11)
    fig.tight_layout()
    out_path = Path(out_path)
    out_path.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(out_path, dpi=150)
    plt.close(fig)
    return out_path


def main() -> None:
    from amsblocks.costs import check_case2

    here = Path(__file__).resolve().parent.parent
    fig_path = here / "figures" / "scaling_check.png"
    plot_scaling(fig_path)
    print(f"wrote {fig_path}")

    print("\nBandwidth anchors (re-derived = input, by construction):")
    for node, hz in BANDWIDTH_ANCHORS:
        got = float(fabric_bandwidth_hz(node))
        print(f"  {node:g} nm -> {got:.3e} Hz  (paper {hz:.3e})")

    c3 = check_comm_case3()
    print("\nA.4 comm Case III (2 GHz on 2x3 mm 40 nm FPAA):")
    print(f"  model bandwidth {c3['model_bandwidth_hz']:.3e} Hz, "
          f"metric {c3['paper_metric_hz']:.3e} Hz, "
          f"met={c3['met']} ({c3['margin_x']:.2f}x)")

    print("\nCase II CAB arithmetic:")
    r = check_case2()
    print(f"  WTA CABs {r['wta_cabs']:.1f} -> {r['wta_vs_one_chip']['chips']:.2f} chips  "
          f"barely>1={r['wta_claim_holds']}")
    print(f"  full program {r['breakdown']['cabs']:.1f} CABs -> "
          f"{r['full_program_vs_one_chip']['chips']:.2f} chips  "
          f"fits-in-2={r['full_claim_holds']}")
    print(f"  weights {r['weights']}")


if __name__ == "__main__":
    main()
