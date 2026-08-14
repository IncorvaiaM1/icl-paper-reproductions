"""Fig 18 CAB-cost table (TRETS 2024) and the Case II acoustic arithmetic check.

Sources
-------
Hasler & Hao, TRETS 17:1 Art. 12 (2024), Fig 18 and §4 / Fig 17.

SoC FPAA CAB count is not printed in TRETS. We use 98 CABs from the SoC
FPAA paper the table is baselined to (TRETS ref [31], Hasler 2016 JETCAS).
That assumption is what makes the published 'barely > 1 chip, fits in 2'
claim land; it is called out in FINDINGS_ams.md.

VMMWTA on the SoC FPAA is 1 output / CAB (body text p. 12:14) and
'13 in + 1 out/CAB' (Fig 18). Body text also says next-gen standard cells
would be 8–16 outputs / CAB — we do not use that for the SoC check.
"""

from __future__ import annotations

from dataclasses import dataclass, field
from math import ceil
from typing import Any


# --- Fig 18, baselined to SoC FPAA CABs ---------------------------------

SOC_FPAA_CABS = 98  # Hasler 2016 JETCAS [TRETS ref 31]; not restated in TRETS
SOC_FPAA_NODE_NM = 350.0
WTA_OUT_PER_CAB_SOC = 1.0  # TRETS p. 12:14
VMM_IN_PER_CAB = 13.0  # Fig 18: "13in + 1 out/CAB" for VMMWTA

# CAB cost per *instance* (one vector channel / one stage), SoC FPAA units.
# Utilities that only reshape buses cost 0.
FIG18: dict[str, float] = {
    "LPF": 0.25,  # 1/4 CAB (TA)
    "HopfLPF": 1.0,  # 1 CAB (4 TA)
    "LadderF": 1.0,  # 1 CAB / stage (2 FG TA)
    "LadderFM": 1.0,  # same per-stage rate as LadderF; Fig 18 defers to [75]
    "BPF": 0.5,  # 1/2 CAB (2 TA)
    "AdaptBPF": 0.75,  # 3/4 CAB (3 TA)
    "AmplitudeBPF": 1.0,  # 1 CAB (4 TA)
    "AmplitudeAdaptBPF": 1.0,  # 1 CAB (4 TA)
    "VMM": 0.0,  # routing ("R") + 4 TA; counted via in/out density below
    "VMMWTA": 0.0,  # routing + 2 nFETs; counted via 1 out/CAB
    "SigVMM": 0.0,
    "M2V": 0.0,
    "MergeV": 0.0,
}


@dataclass
class CabBreakdown:
    cabs: float
    parts: dict[str, float] = field(default_factory=dict)
    notes: list[str] = field(default_factory=list)

    def as_dict(self) -> dict[str, Any]:
        return {"cabs": self.cabs, "parts": dict(self.parts), "notes": list(self.notes)}


def _vmmwta_cabs(n_in: int, n_out: int) -> float:
    """SoC FPAA: 1 WTA output / CAB, 13 inputs / CAB. Take the max."""
    by_out = n_out / WTA_OUT_PER_CAB_SOC
    by_in = n_in / VMM_IN_PER_CAB
    return max(by_out, by_in)


def cab_cost(program: dict[str, Any]) -> CabBreakdown:
    """Cost a level=1 block program.

    ``program`` keys are Fig 15 names. Values are dicts of sizes, e.g.::

        {
          "AmplitudeBPF": {"n": 12},
          "LadderFM": {"m": 12, "n_stages": 3},
          "M2V": {},
          "VMMWTA": {"n_in": 48, "n_out": 128},
        }
    """
    parts: dict[str, float] = {}
    notes: list[str] = []
    total = 0.0

    if "AmplitudeBPF" in program:
        n = int(program["AmplitudeBPF"]["n"])
        parts["AmplitudeBPF"] = FIG18["AmplitudeBPF"] * n
        total += parts["AmplitudeBPF"]

    if "BPF" in program:
        n = int(program["BPF"]["n"])
        parts["BPF"] = FIG18["BPF"] * n
        total += parts["BPF"]

    if "LadderFM" in program:
        spec = program["LadderFM"]
        m = int(spec.get("m", spec.get("n_channels", 1)))
        n_stages = int(spec.get("n_stages", spec.get("n", 1)))
        # LadderF is 1 CAB/stage. LadderFM is the multi-input form; we cost
        # 1 CAB/stage/channel (parallel ladders). Documented assumption.
        parts["LadderFM"] = FIG18["LadderFM"] * m * n_stages
        total += parts["LadderFM"]
        notes.append(
            "LadderFM costed as 1 CAB/stage/channel (Fig 18 LadderF rate; "
            "LadderFM itself is a pointer to [75])."
        )

    if "LadderF" in program:
        n_stages = int(program["LadderF"].get("n_stages", program["LadderF"].get("n", 1)))
        parts["LadderF"] = FIG18["LadderF"] * n_stages
        total += parts["LadderF"]

    if "VMMWTA" in program:
        n_in = int(program["VMMWTA"]["n_in"])
        n_out = int(program["VMMWTA"]["n_out"])
        parts["VMMWTA"] = _vmmwta_cabs(n_in, n_out)
        total += parts["VMMWTA"]
        notes.append(
            f"VMMWTA SoC: {WTA_OUT_PER_CAB_SOC:g} out/CAB, "
            f"{VMM_IN_PER_CAB:g} in/CAB -> max({n_out:g}, {n_in}/{VMM_IN_PER_CAB:g})."
        )

    if "VMM" in program:
        n_in = int(program["VMM"]["n_in"])
        n_out = int(program["VMM"]["n_out"])
        parts["VMM"] = max(n_out / 2.0, n_in / VMM_IN_PER_CAB)  # Fig 18: 13in + 2 out/CAB
        total += parts["VMM"]

    for name in ("M2V", "MergeV"):
        if name in program:
            parts[name] = 0.0

    return CabBreakdown(cabs=total, parts=parts, notes=notes)


def case2_acoustic_program() -> dict[str, Any]:
    """Fig 17–18 worked example: 12-band AFE, 3 delays → 48 lines, 48×128 VMM+WTA.

    12 bands × (undelayed + 3 delay taps) = 48 inputs. 32 words + 96 nulls = 128
    WTA outputs. 48 × 128 = 6,144 weights. Matches TRETS §4 / Fig 17–18.
    """
    return {
        "AmplitudeBPF": {"n": 12},
        "LadderFM": {"m": 12, "n_stages": 3},
        "M2V": {},
        "VMMWTA": {"n_in": 48, "n_out": 128},
    }


def case1_acoustic_program(n_bands: int = 16, n_stages: int = 3, n_words: int = 4) -> dict[str, Any]:
    """JLPEA Fig 3 Case I: 16 subbands, ladder taps, VMM+WTA over ``n_words``."""
    n_in = n_bands * (n_stages + 1)
    return {
        "AmplitudeBPF": {"n": n_bands},
        "LadderFM": {"m": n_bands, "n_stages": n_stages},
        "M2V": {},
        "VMMWTA": {"n_in": n_in, "n_out": n_words},
    }


def chips_needed(cabs: float, cabs_per_chip: float = SOC_FPAA_CABS) -> dict[str, Any]:
    n = cabs / cabs_per_chip
    return {
        "cabs": cabs,
        "cabs_per_chip": cabs_per_chip,
        "chips": n,
        "chips_ceil": int(ceil(n)),
        "barely_over_one": n > 1.0 and n < 2.0,
        "fits_in_two": n <= 2.0,
    }


def check_case2() -> dict[str, Any]:
    """Reproduce the §4 claim: more WTA than one SoC FPAA (barely), fits in two."""
    prog = case2_acoustic_program()
    bd = cab_cost(prog)
    wta_only = _vmmwta_cabs(48, 128)
    full = chips_needed(bd.cabs)
    wta = chips_needed(wta_only)
    claim = {
        "program": prog,
        "breakdown": bd.as_dict(),
        "wta_cabs": wta_only,
        "wta_vs_one_chip": wta,
        "full_program_vs_one_chip": full,
        "weights": 48 * 128,
        "paper_claim": (
            "48-in × 128-out VMM+WTA with a 6,144-weight matrix; more WTA "
            "stages than fit on one SoC FPAA (barely); two 350 nm SoC FPAAs; "
            "~250 µm × 300 µm in 130 nm std cells."
        ),
        "wta_claim_holds": wta["barely_over_one"] and wta["fits_in_two"],
        "full_claim_holds": full["fits_in_two"],
    }
    return claim


def _print_case2() -> None:
    r = check_case2()
    print("Case II acoustic CAB check (TRETS Fig 17-18)")
    print(f"  weights: {r['weights']} (paper: 6144)")
    print(f"  breakdown: {r['breakdown']['parts']}")
    print(f"  total CABs: {r['breakdown']['cabs']:.1f}")
    print(f"  WTA-only CABs: {r['wta_cabs']:.1f}  -> {r['wta_vs_one_chip']['chips']:.2f} chips")
    print(f"  full program: {r['full_program_vs_one_chip']['chips']:.2f} chips")
    print(f"  WTA barely>1 and fits-in-2: {r['wta_claim_holds']}")
    print(f"  full program fits-in-2: {r['full_claim_holds']}")
    for n in r["breakdown"]["notes"]:
        print(f"  note: {n}")
    print(f"  SoC FPAA CABs assumed: {SOC_FPAA_CABS} (ref [31], not printed in TRETS)")


if __name__ == "__main__":
    _print_case2()
