# Day-0 Findings — HLS-Eval toolchain audit

Everything below was produced **before running a single language model**, by
building and running HLS-Eval's own reference designs with a stock compiler.
All of it is reproducible with:

```bash
python scripts/verify_backend.py --data <repo>/hls_eval_data --all --json results/backend_check_all.json
```

Confirmed on two independent toolchains, both with no Vitis HLS present and both
at `-std=c++14 -O2 -Wno-unknown-pragmas`:

| Toolchain | Scope | Result |
|---|---|---|
| GCC 11.4.0 / Ubuntu 22.04 | all 110 designs | 84 compile, 83 run |
| Clang 18.1.3 / Ubuntu 24.04 | polybench + machsuite + c2hlsc (60) | **60 compile, 60 run** |

The 60-design usable set passes at 100% on both a four-year-old GCC and a
current Clang, so the result is not an artifact of one compiler's leniency.
hls-eval @ `adea9ff` (2026-07-30).

---

## 1. Three of the four metrics do not need Vitis

HLS-Eval's C-simulation path goes through the clang bundled inside a Vitis
installation (`<vitis>/lnx64/tools/clang-3.9-csynth/bin/clang++`), which makes a
~60 GB Linux Vitis install a prerequisite for every metric.

It isn't one. Of 110 reference designs, only **13 reference any Xilinx-specific
type** (`ap_int`, `ap_uint`, `ap_fixed`, `hls_stream`, `hls::`). The rest are
plain C++. `#pragma HLS ...` is ignored as an unknown pragma by a standard
compiler, so generated code carrying pragmas still builds.

| Metric | Needs Vitis |
|---|---|
| Parseable | No — pure string extraction, no compiler |
| Compilable | No — stock `clang++` / `g++` |
| Runnable | No — testbenches are plain C++ |
| Synthesizable | **Yes** |

`ext/stock_tools.py` implements this as a drop-in for `VitisHLSCSimTool`.

---

## 2. Reference-design results, all 110 designs, no Vitis

| Suite | Compile | Run | Uses HLS types | Verdict |
|---|---|---|---|---|
| **polybench** | 28/28 | **28/28** | 0 | fully viable |
| **machsuite** | 18/18 | **18/18** | 0 | fully viable |
| **c2hlsc** | 14/14 | **14/14** | 0 | fully viable |
| chstone | 20/32 | 20/32 | 0 | 12 have no testbench (§3) |
| gnnbuilder | 3/3 | 2/3 | 0 | 1 testbench overflows (§5) |
| pp4fpga | 1/3 | 1/3 | 2 | needs Xilinx headers |
| flowgnn | 0/3 | 0/3 | 2 | 2 headers + 1 missing include (§4) |
| rosetta | 0/9 | 0/9 | 9 | needs Xilinx headers |
| **Total** | **84/110** | **83/110** | 13 | |

**60 designs across polybench, machsuite and c2hlsc pass at 100% with no Xilinx
software installed at all.** That is the usable evaluation set for a Vitis-free
run, and it covers three of the five suites the paper's main experiment used.

The 13 designs needing Xilinx types only need the *headers* (`<vitis>/include`),
not the compiler or the shared libraries — `StockCSimTool` takes a
`vitis_hls_path` and will pick those up if a install is present.

---

## 3. Twelve chstone designs ship without a testbench

Every chstone compile failure is `undefined reference to 'main'`. The cause is
not the toolchain: these 12 directories contain only a kernel, a header, and
`top.txt` — no `*_tb.cpp`.

```
adpcm__filtep   adpcm__filtez   adpcm__logsch   adpcm__logscl
adpcm__quantl   adpcm__scalel   adpcm__uppol1   adpcm__uppol2
adpcm__upzero   df_sub128       jpeg__YuvToRgb  motion__decode_motion_vector
```

The set of link failures and the set of testbench-less directories match
exactly, 12 for 12.

**Consequence:** the *runnable* metric is undefined for these designs. They can
still be parsed, compiled as a translation unit, and synthesized — synthesis
needs no `main` — but they cannot be C-simulated. If they are scored as
"failed to run," chstone's runnability is understated by 12/32 = 37.5%.

`scripts/run_experiment.py` excludes them by name and records the exclusion in
the run manifest.

---

## 4. `flowgnn/fgnn_linear` testbench is missing an include

```
linear_tb.cpp:23:9: error: 'printf' was not declared in this scope
   23 |         printf("%f\n", output[i]);
      note: 'printf' is defined in header '<cstdio>'
```

The testbench includes only `"linear.h"` and calls `printf`. Vitis's clang-3.9
pulls `<cstdio>` in transitively; GCC 11 and modern Clang do not. One-line fix:
add `#include <cstdio>`.

This design does *not* use HLS types — it is the only flowgnn case blocked
purely by a missing include rather than by Xilinx headers.

---

## 5. `gnnbuilder/gather_node_neighbors` testbench overflows its buffer

Compiles clean, then dies at runtime:

```
gather_node_neighbors_tb.cpp:12:31: warning: iteration 993 invokes undefined
   behavior [-Waggressive-loop-optimizations]
   12 |         neighbor_table[7 + i] = i;

*** stack smashing detected ***: terminated
```

The initialization loop writes `neighbor_table[7 + i]` for `i` running past the
array's capacity — an out-of-bounds write in the reference testbench, caught by
GCC's stack protector. It presumably went unnoticed because Vitis's clang-3.9
does not enable the protector by default and the overflow is silent there.

Any model scored against this design is scored against a testbench that
corrupts its own stack.

---

## 6. Two robustness gaps in the harness, relevant once inputs are LLM-generated

Both are in `hls_eval/tools.py` and both are benign for reference designs but
not for generated ones:

**No timeout on the execution step.** `CPPCompilerTool.run` bounds compilation
but runs the produced binary with a bare `subprocess.run([out_bin])` — no
timeout. One generated infinite loop hangs a worker for the rest of the sweep.

**Execution inherits the parent's working directory.** The tool copies sources
and aux files into a per-build directory, then runs the binary without setting
`cwd`. Any testbench opening a data file by relative path reads from the wrong
place. Latent on polybench, which does no file I/O; live on suites that do.

`ext/stock_tools.py` sets `cwd` to the build directory, bounds execution with a
timeout, and caps the child's address space so a runaway allocation cannot take
the machine down.

---

## What this is worth

Two fixable bugs in reference designs, one benchmark property that biases a
metric if unhandled, and two harness robustness gaps — all surfaced by running
the existing code with a modern compiler, none requiring a model or a license.

It also means the reproduction can start immediately on 60 designs rather than
waiting on a 60 GB download.
