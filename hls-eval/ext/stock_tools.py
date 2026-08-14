"""
Vitis-free backends for the HLS-Eval harness.

Motivation
----------
`hls_eval.tools.VitisHLSCSimTool` and `CPPCompilerTool` both shell out to the
clang that ships inside a Vitis HLS installation:

    <vitis>/lnx64/tools/clang-3.9-csynth/bin/clang++
    <vitis>/lib/lnx64.o
    <vitis>/lnx64/lib/csim

That makes a ~60 GB Linux Vitis install a hard prerequisite for *all four* of the
paper's metrics. It does not need to be.

Verified against the shipped benchmark data: no design under
`hls_eval_data/polybench/` references `ap_int`, `ap_fixed`, `hls_stream`, or any
`hls::` symbol. They are plain C++ over `double` arrays. `#pragma HLS ...` is
ignored as an unknown pragma by a standard compiler. So:

    parseable    -> no compiler at all (pure string extraction)
    compilable   -> stock clang++/g++
    runnable     -> stock clang++/g++
    synthesizable-> Vitis, genuinely required

`StockCSimTool` is a drop-in for `VitisHLSCSimTool` covering the first three.
`NullSynthTool` stands in for `VitisHLSSynthTool` so the evaluator — which calls
synthesis unconditionally — does not crash when Vitis is absent.

    >>> from ext.stock_tools import StockCSimTool, NullSynthTool
    >>> evaluator = HLSGenerationZeroShotEvaluator(
    ...     vitis_hls_tool_csim=StockCSimTool(),
    ...     vitis_hls_tool_synth=NullSynthTool(),
    ...     output_data_dir=OUT,
    ... )

WARNING
-------
`NullSynthTool` emits `return_code == SYNTH_SKIPPED` (-99), never 0. Any analysis
that treats "synthesis return code != 0" as a synthesis *failure* will report 0%
synthesizability, which is a false result rather than a missing one. Filter on
the sentinel before aggregating. `ext/analysis_guard.py` does this for you.
"""

from __future__ import annotations

import io
import os
import resource
import shutil
import subprocess
import time
import uuid
from pathlib import Path

import psutil

from hls_eval.data import H_EXTENSIONS
from hls_eval.tools import ExecutionData, ToolDataOutput

SYNTH_SKIPPED = -99
"""Sentinel return code meaning 'synthesis was never attempted', not 'failed'."""

_PREFERRED_COMPILERS = ("clang++", "g++")


def detect_compiler(preferred: str | None = None) -> Path:
    """Locate a usable host C++ compiler.

    clang++ is preferred over g++ only because Vitis's own csim compiler is a
    clang derivative, so diagnostics land closer to what the paper's harness
    would have produced.
    """
    candidates = (preferred,) if preferred else _PREFERRED_COMPILERS
    for name in candidates:
        if name is None:
            continue
        found = shutil.which(name)
        if found:
            return Path(found)
    raise RuntimeError(
        f"No C++ compiler found (looked for: {', '.join(c for c in candidates if c)}). "
        "In WSL/Ubuntu: sudo apt install -y clang"
    )


def _limit_child(memory_limit_mb: int | None):
    """Cap address space for generated code. LLM output can allocate absurdly."""
    if memory_limit_mb is None:
        return None

    def _apply() -> None:
        n_bytes = memory_limit_mb * 1024 * 1024
        resource.setrlimit(resource.RLIMIT_AS, (n_bytes, n_bytes))
        resource.setrlimit(resource.RLIMIT_CORE, (0, 0))

    return _apply


def _kill_tree(popen: subprocess.Popen) -> None:
    try:
        parent = psutil.Process(pid=popen.pid)
        for child in parent.children(recursive=True):
            child.kill()
    except psutil.NoSuchProcess:
        pass
    popen.kill()


def _timeout_output(t0: float, timeout: float) -> ToolDataOutput:
    return ToolDataOutput(
        data_execution=ExecutionData(
            return_code=-1,
            stdout="",
            stderr=f"timeout after {timeout}s",
            t0=t0,
            t1=time.monotonic(),
            execution_time=timeout,
            timeout=True,
        ),
        data_tool=None,
    )


def _run_capture(
    cmd: list[str],
    cwd: Path,
    timeout: float,
    env: dict | None = None,
    memory_limit_mb: int | None = None,
) -> ToolDataOutput:
    """Run a command, always returning a ToolDataOutput rather than raising."""
    t0 = time.monotonic()
    popen = subprocess.Popen(
        cmd,
        cwd=str(cwd),
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        errors="replace",
        bufsize=io.DEFAULT_BUFFER_SIZE * 1024,
        env=env,
        preexec_fn=_limit_child(memory_limit_mb),
    )
    try:
        stdout, stderr = popen.communicate(timeout=timeout)
    except subprocess.TimeoutExpired:
        _kill_tree(popen)
        popen.communicate()
        return _timeout_output(t0, timeout)

    t1 = time.monotonic()
    return ToolDataOutput(
        data_execution=ExecutionData(
            return_code=popen.returncode,
            stdout=stdout or "",
            stderr=stderr or "",
            t0=t0,
            t1=t1,
            execution_time=t1 - t0,
            timeout=False,
        ),
        data_tool=None,
    )


class StockCSimTool:
    """Compile-and-run backend using the host C++ compiler instead of Vitis's.

    Signature-compatible with `hls_eval.tools.VitisHLSCSimTool.run`; the
    HLS-specific keyword arguments are accepted and ignored so this drops into
    the existing evaluator without touching upstream code.

    Two deliberate differences from upstream `CPPCompilerTool`, both of which
    matter once the code under test is LLM-generated rather than a reference
    design:

    1. The compiled binary runs with ``cwd`` set to the build directory.
       Upstream leaves cwd at the parent process's, so any testbench opening a
       data file by relative path reads from the wrong place. Polybench
       testbenches happen not to do file I/O, so this is latent there, but it
       bites on rosetta and chstone.
    2. Execution is bounded by a timeout and an address-space cap. Upstream has
       no timeout on the run step at all; a single generated infinite loop will
       hang a worker thread for the rest of the sweep.
    """

    def __init__(
        self,
        compiler: str | Path | None = None,
        std: str = "c++14",
        opt_level: str = "-O2",
        extra_include_dirs: tuple[Path, ...] = (),
        vitis_hls_path: Path | None = None,
        compile_timeout: float = 120.0,
        run_timeout: float = 60.0,
        memory_limit_mb: int | None = 4096,
    ) -> None:
        self.compiler = (
            Path(compiler) if compiler and Path(compiler).exists() else detect_compiler(
                str(compiler) if compiler else None
            )
        )
        self.std = std
        self.opt_level = opt_level
        self.compile_timeout = compile_timeout
        self.run_timeout = run_timeout
        self.memory_limit_mb = memory_limit_mb

        # If a Vitis install happens to be present, borrow only its headers.
        # This lets designs that DO use ap_int/hls_stream still compile, without
        # depending on Vitis's clang or its shared libraries.
        include_dirs = list(extra_include_dirs)
        if vitis_hls_path is not None:
            vitis_include = Path(vitis_hls_path) / "include"
            if vitis_include.is_dir():
                include_dirs.append(vitis_include)
        self.include_dirs = tuple(include_dirs)

    def describe(self) -> str:
        version = subprocess.run(
            [str(self.compiler), "--version"], capture_output=True, text=True
        ).stdout.splitlines()
        head = version[0] if version else str(self.compiler)
        inc = f", headers: {[str(p) for p in self.include_dirs]}" if self.include_dirs else ""
        return f"StockCSimTool({head}, -std={self.std}{inc})"

    def _cflags(self) -> list[str]:
        flags = [
            f"-std={self.std}",
            self.opt_level,
            "-Wno-unknown-pragmas",  # '#pragma HLS ...' is expected and harmless
            "-Wno-unused-function",
            "-fno-strict-aliasing",
        ]
        for d in self.include_dirs:
            flags.append(f"-I{d}")
        return flags

    def run(
        self,
        build_dir: Path,
        source_files: list[Path],
        aux_files: list[Path] = [],
        build_name: str | None = None,
        build_name_prefix: str = "stock_csim_tool__",
        **_ignored_hls_kwargs,
    ) -> tuple[ToolDataOutput, ToolDataOutput | None]:
        name = build_name or uuid.uuid4().hex
        unique_build_dir = Path(build_dir) / f"{build_name_prefix}{name}"
        if unique_build_dir.exists():
            shutil.rmtree(unique_build_dir)
        unique_build_dir.mkdir(parents=True, exist_ok=True)

        for fp in list(source_files) + list(aux_files):
            try:
                shutil.copy(fp, unique_build_dir)
            except (OSError, shutil.SameFileError):
                pass

        translation_units = [
            fp for fp in source_files if Path(fp).suffix not in H_EXTENSIONS
        ]
        if not translation_units:
            t0 = time.monotonic()
            return ToolDataOutput(
                data_execution=ExecutionData(
                    return_code=1,
                    stdout="",
                    stderr="no .cpp translation units to compile",
                    t0=t0,
                    t1=t0,
                    execution_time=0.0,
                    timeout=False,
                ),
                data_tool=None,
            ), None

        out_bin = unique_build_dir / "out"
        cmd = [str(self.compiler)]
        cmd += [str(Path(fp).resolve()) for fp in translation_units]
        cmd += ["-o", str(out_bin.resolve())]
        cmd += self._cflags()
        cmd += ["-lm"]

        compile_out = _run_capture(
            cmd,
            cwd=unique_build_dir,
            timeout=self.compile_timeout,
            env=os.environ.copy(),
        )
        if compile_out.data_execution.return_code != 0 or not out_bin.exists():
            return compile_out, None

        run_out = _run_capture(
            [str(out_bin.resolve())],
            cwd=unique_build_dir,
            timeout=self.run_timeout,
            env=os.environ.copy(),
            memory_limit_mb=self.memory_limit_mb,
        )
        return compile_out, run_out


class NullSynthTool:
    """Stand-in for `VitisHLSSynthTool` when no Vitis install is available.

    The evaluator submits a synthesis job unconditionally, so it needs *some*
    object here. This one records that synthesis was skipped and returns
    immediately.

    Emits `SYNTH_SKIPPED` (-99), deliberately not 0 and not 1, so that
    downstream analysis can distinguish "not attempted" from "attempted and
    failed". Reporting these as failures would understate synthesizability to
    exactly 0%.
    """

    def __init__(self, reason: str = "no Vitis HLS installation available") -> None:
        self.reason = reason

    def describe(self) -> str:
        return f"NullSynthTool(skipped: {self.reason})"

    def run(self, *_args, **_kwargs) -> ToolDataOutput:
        t = time.monotonic()
        return ToolDataOutput(
            data_execution=ExecutionData(
                return_code=SYNTH_SKIPPED,
                stdout="",
                stderr=f"SYNTHESIS SKIPPED: {self.reason}",
                t0=t,
                t1=t,
                execution_time=0.0,
                timeout=False,
            ),
            data_tool=None,
        )
