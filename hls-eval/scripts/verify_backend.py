#!/usr/bin/env python3
"""
Sanity-check the Vitis-free C-sim path against HLS-Eval's *reference* designs.

This must be run before any LLM is involved. The reference implementations that
ship with the benchmark are known-good by construction, so they should compile
and run at 100%. Any design that fails here is a property of the toolchain, not
of a model — and any model's score on that design is meaningless until it's
fixed or excluded.

Standalone on purpose: no `hls_eval` import, so it runs before `uv sync`.
It mirrors what `ext/stock_tools.StockCSimTool` does.

    python scripts/verify_backend.py --data /path/to/hls-eval/hls_eval_data
    python scripts/verify_backend.py --data ... --suite polybench --suite machsuite
    python scripts/verify_backend.py --data ... --all --json results/backend_check.json
"""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
import tempfile
import time
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

H_EXTENSIONS = {".h", ".hpp", ".hh"}
HLS_TYPE_MARKERS = ("ap_int", "ap_uint", "ap_fixed", "hls_stream", "hls::")


def detect_compiler(preferred: str | None = None) -> str:
    for name in ([preferred] if preferred else []) + ["clang++", "g++"]:
        if name and shutil.which(name):
            return shutil.which(name)  # type: ignore[return-value]
    sys.exit("No C++ compiler found. In WSL/Ubuntu: sudo apt install -y clang")


def uses_hls_types(design_dir: Path) -> bool:
    for fp in list(design_dir.glob("*.cpp")) + list(design_dir.glob("*.h")):
        try:
            text = fp.read_text(errors="ignore")
        except OSError:
            continue
        if any(m in text for m in HLS_TYPE_MARKERS):
            return True
    return False


def check_design(
    design_dir: Path,
    compiler: str,
    std: str,
    vitis_include: Path | None,
    compile_timeout: float,
    run_timeout: float,
) -> dict:
    result: dict = {
        "design": design_dir.name,
        "suite": design_dir.parent.name,
        "needs_hls_headers": uses_hls_types(design_dir),
        "compile_rc": None,
        "run_rc": None,
        "compile_s": None,
        "run_s": None,
        "error": "",
    }

    sources = sorted(p for p in design_dir.glob("*.cpp") if p.suffix not in H_EXTENSIONS)
    if not sources:
        result["error"] = "no .cpp sources"
        return result

    with tempfile.TemporaryDirectory(prefix=f"vb_{design_dir.name}_") as tmp:
        work = Path(tmp)
        for fp in design_dir.iterdir():
            if fp.is_file():
                shutil.copy(fp, work)

        out_bin = work / "out"
        cmd = [compiler, *[str(work / s.name) for s in sources], "-o", str(out_bin)]
        cmd += [
            f"-std={std}",
            "-O2",
            "-Wno-unknown-pragmas",
            "-Wno-unused-function",
            "-fno-strict-aliasing",
        ]
        if vitis_include:
            cmd.append(f"-I{vitis_include}")
        cmd.append("-lm")

        t0 = time.monotonic()
        try:
            p = subprocess.run(
                cmd, cwd=work, capture_output=True, text=True,
                errors="replace", timeout=compile_timeout,
            )
            result["compile_rc"] = p.returncode
            if p.returncode != 0:
                result["error"] = (p.stderr or "").strip().splitlines()[-1][:200] if p.stderr else "compile failed"
        except subprocess.TimeoutExpired:
            result["compile_rc"] = -1
            result["error"] = "compile timeout"
        result["compile_s"] = round(time.monotonic() - t0, 2)

        if result["compile_rc"] != 0 or not out_bin.exists():
            return result

        t0 = time.monotonic()
        try:
            p = subprocess.run(
                [str(out_bin)], cwd=work, capture_output=True, text=True,
                errors="replace", timeout=run_timeout,
            )
            result["run_rc"] = p.returncode
            if p.returncode != 0:
                result["error"] = f"nonzero exit {p.returncode}"
        except subprocess.TimeoutExpired:
            result["run_rc"] = -1
            result["error"] = "run timeout"
        result["run_s"] = round(time.monotonic() - t0, 2)

    return result


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--data", required=True, type=Path, help="path to hls_eval_data/")
    ap.add_argument("--suite", action="append", default=[], help="repeatable; default polybench")
    ap.add_argument("--all", action="store_true", help="every suite")
    ap.add_argument("--compiler", default=None)
    ap.add_argument("--std", default="c++14")
    ap.add_argument("--vitis-include", type=Path, default=None,
                    help="optional <vitis>/include, enables ap_int/hls_stream designs")
    ap.add_argument("--jobs", type=int, default=8)
    ap.add_argument("--compile-timeout", type=float, default=120.0)
    ap.add_argument("--run-timeout", type=float, default=60.0)
    ap.add_argument("--json", type=Path, default=None)
    args = ap.parse_args()

    if not args.data.is_dir():
        sys.exit(f"not a directory: {args.data}")

    suites = (
        sorted(d.name for d in args.data.iterdir() if d.is_dir())
        if args.all
        else (args.suite or ["polybench"])
    )

    designs = [
        d for s in suites for d in sorted((args.data / s).glob("*/")) if d.is_dir()
    ]
    if not designs:
        sys.exit(f"no designs found under {args.data} for suites {suites}")

    compiler = detect_compiler(args.compiler)
    ver = subprocess.run([compiler, "--version"], capture_output=True, text=True).stdout
    print(f"compiler : {ver.splitlines()[0] if ver else compiler}")
    print(f"std      : {args.std}")
    print(f"vitis inc: {args.vitis_include or '(none)'}")
    print(f"suites   : {', '.join(suites)}  ({len(designs)} designs)\n")

    with ThreadPoolExecutor(max_workers=args.jobs) as pool:
        results = list(pool.map(
            lambda d: check_design(
                d, compiler, args.std, args.vitis_include,
                args.compile_timeout, args.run_timeout,
            ),
            designs,
        ))
    results.sort(key=lambda r: (r["suite"], r["design"]))

    print(f"{'suite':<12} {'design':<34} {'cc':>4} {'run':>4} {'s':>6}  note")
    print("-" * 88)
    for r in results:
        ok = r["compile_rc"] == 0 and r["run_rc"] == 0
        mark = "" if ok else "  <-- FAIL"
        note = (r["error"] or "")[:28]
        if r["needs_hls_headers"] and not args.vitis_include:
            note = note or "uses HLS types"
        total = (r["compile_s"] or 0) + (r["run_s"] or 0)
        print(
            f"{r['suite']:<12} {r['design']:<34} "
            f"{str(r['compile_rc']):>4} {str(r['run_rc']):>4} {total:>6.1f}  {note}{mark}"
        )

    n = len(results)
    n_cc = sum(1 for r in results if r["compile_rc"] == 0)
    n_run = sum(1 for r in results if r["run_rc"] == 0)
    n_hls = sum(1 for r in results if r["needs_hls_headers"])

    print("\n" + "=" * 88)
    print(f"compiled : {n_cc}/{n}  ({100 * n_cc / n:.1f}%)")
    print(f"ran      : {n_run}/{n}  ({100 * n_run / n:.1f}%)")
    print(f"need HLS headers: {n_hls}/{n}")

    by_suite: dict[str, list] = {}
    for r in results:
        by_suite.setdefault(r["suite"], []).append(r)
    if len(by_suite) > 1:
        print("\nper suite:")
        for s, rs in sorted(by_suite.items()):
            c = sum(1 for r in rs if r["compile_rc"] == 0)
            v = sum(1 for r in rs if r["run_rc"] == 0)
            h = sum(1 for r in rs if r["needs_hls_headers"])
            print(f"  {s:<12} compile {c:>3}/{len(rs):<3} run {v:>3}/{len(rs):<3} hls-types {h}")

    if args.json:
        args.json.parent.mkdir(parents=True, exist_ok=True)
        args.json.write_text(json.dumps(
            {
                "compiler": ver.splitlines()[0] if ver else compiler,
                "std": args.std,
                "vitis_include": str(args.vitis_include) if args.vitis_include else None,
                "suites": suites,
                "summary": {"n": n, "compiled": n_cc, "ran": n_run, "need_hls_headers": n_hls},
                "results": results,
            },
            indent=2,
        ))
        print(f"\nwrote {args.json}")

    if n_run != n:
        print("\nNOT all reference designs pass. Exclude the failures from any")
        print("model evaluation, or the scores for those designs are meaningless.")
        return 1
    print("\nAll reference designs compile and run. Backend is trustworthy for this suite.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
