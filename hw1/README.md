# Homework 1 – Matrix Multiplication Profiling

This repo contains a simple CPU‑bound dense matrix multiplication kernel and scripts to collect performance data.

## Goals

- Implement a baseline O(N³) multiplication: `C = A × B` with random inputs.
- Parameterise the problem size with `N` (e.g. 1024, 2048, 4096).
- Automate compilation and profiling with the Makefile; use RUNS for repeated measurements when profiling.
- Use `gprof` and `perf` on Linux to profile and understand where time is spent.
- Explore how data type, matrix size, loop order, tiling and unrolling affect performance.

## Environment Preparation

Install a C++ toolchain with `g++`, `gprof` and `perf` (e.g. `apt install build-essential binutils linux-tools-common linux-tools-$(uname -r)`).

To compile and run the baseline code:

```sh
# clean previous binaries
make clean

# build and run once with default N=1024
make run

# profile with gprof and compute average over RUNS
make gprof RUNS=3

# collect low‑level counters (cycles, instructions, cache misses) averaged over RUNS
sudo make perf RUNS=3

# run the predefined experiment matrix (4‑tuples) in the Makefile
make part1
```

## Changing parameters

The Makefile exposes several variables you can override on the command line:

 • **N** – the matrix dimension.  Default is 1024; override it to build a different sized kernel, e.g. `make run N=2048`.

 • **RUNS** – the number of repetitions for profiling. Use it with `gprof` and `perf`, or set it per-row in the experiment matrix for `part1`. Examples: `make gprof RUNS=5`, `sudo make perf RUNS=10`.

 • **PROG** – the program to compile, corresponding to the C++ source name without extension.  Place your variants in the `src/` folder and run them by setting `PROG`, e.g. `make run PROG=matmul_double`.

 • **EXPERIMENTS** – the 4‑tuple matrix (prog,size,runs,tools) used by the `part1` target. Edit it in the `Makefile` to control which programs/sizes are executed and which tools run.

## Directory structure

This project separates source code, binaries and results:

 • **src/** – all C++ source files live here (`matmul_int.cpp`, `matmul_double.cpp`, etc.).

 • **bin/** – compiled binaries are written here, with names that encode the program and N (e.g. `matmul_int_N1024`).

 • **results/gprof/** – text reports from `gprof` (invoked via `make gprof`), organized by program.

 • **results/perf/** – performance counter stats collected by `perf` (invoked via `sudo make perf`), organized by program.

These directories are created automatically by the Makefile.  When you commit your results for the homework, include the contents of the results tree.

## Part 1 experiment matrix

`make part1` iterates over a list of 4‑tuples defined in the `Makefile`:

```
EXPERIMENTS := \
	matmul_double,1024,3,all \
	matmul_int,1024,3,all \
	matmul_int,2048,3,all \
	matmul_int,4096,3,all
```

Each tuple is `prog,size,runs,tools`:
- **prog**: source variant in `src/` (without `.cpp`).
- **size**: value for `N`.
- **runs**: how many repetitions to average for profiling.
- **tools**: `all`, `gprof`, or `perf`.

Edit `EXPERIMENTS` to customize which experiments run and how they’re profiled.

## Int vs Double @ N=1024 (RUNS=3)

From `perf stat -r3` on the same machine:

| Metric | double (`matmul_double_N1024`) | int (`matmul_int_N1024`) | Delta (int vs double) |
|---|---:|---:|---:|
| **Time elapsed** | 8.260 s (±1.72%) | 7.334 s (±2.87%) | **11.2% faster** |
| **Cycles** | 21,487,506,992 (±1.28%) | 18,729,070,719 (±1.26%) | −12.8% |
| **Instructions** | 1,680,802,088 (±0.11%) | 1,129,007,328 (±0.10%) | −32.8% |
| **IPC** | 0.08 | 0.06 | lower (both are memory‑bound) |
| **Cache misses (count)** | 103,937,526 | 33,871,214 | −67.4% |
| **Cache‑miss rate** | 21.33% | 10.19% | −11.14 pp (≈−52%) |
| **Cache refs** | 487,291,625 | 332,366,680 | −31.8% |
| **L1 d‑cache load misses** | 829,401,745 | 632,128,615 | −23.8% |

**Takeaways**
- `int` wins at N=1024 (≈11–13% faster wall‑time) mainly due to **half the memory footprint**, which cuts cache pressure (miss rate ~21% → ~10%).
- Both kernels show **very low IPC** (0.06–0.08), a hallmark of memory‑bound behavior. Compute isn’t the limiter; memory bandwidth/latency is.
- Expect this gap to **widen with larger N** (working set grows → more misses for `double`), until you change the algorithm (blocking/tiling) to improve locality.

> Reproduce: `make PROG=matmul_double N=1024 perf RUNS=3` and `make PROG=matmul_int N=1024 perf RUNS=3`.


## Scaling (int) – N=1024 → N=2048 (RUNS=3)

From `perf stat -r3` on the same machine:

| Metric | N=1024 | N=2048 | Scale (×) |
|---|---:|---:|---:|
| **Time elapsed** | 7.334 s (±2.87%) | 73.50 s (±1.72%) | **10.02×** |
| **Cycles** | 18,729,070,719 | 191,224,113,511 | 10.21× |
| **Instructions** | 1,129,007,328 | 8,366,591,477 | 7.41× |
| **IPC** | 0.06 | 0.04 | ↓ ~33% |
| **Cache‑miss rate** | 10.19% | 16.03% | +5.84 pp |
| **Cache misses (count)** | 33,871,214 | 583,986,233 | 17.24× |
| **Cache refs** | 332,366,680 | 3,644,105,226 | 10.96× |
| **L1 d‑cache load misses** | 632,128,615 | 5,357,088,509 | 8.47× |

**Interpretation**
- Doubling N ideally costs 8× time for an O(N³) kernel. Observed time grew **10.02×** (≈**25% worse** than ideal), driven by a larger working set overflowing caches.
- Miss rate rises from **10.19% → 16.03%** and IPC drops **0.06 → 0.04**, confirming the **memory‑bound** nature becomes more severe at N=2048.
- Instruction count scales ~**7.41×**, not 8×—likely due to constant‑overhead terms (loop setup, branches) becoming less dominant at larger N and compiler effects (unrolling/vectorization).

> Reproduce: `make PROG=matmul_int N=1024 perf RUNS=3` and `make PROG=matmul_int N=2048 perf RUNS=3`.

## Notes on perf

Collecting hardware events requires access to the CPU’s performance monitoring unit (PMU).  On some virtual machines the counters may be unavailable, in which case perf will report events as `<not supported>`.  In particular, running on a macOS host inside Parallels, VMware or other Mac hypervisors does not virtualise the PMU, so perf cannot measure cycles, instructions or cache misses.  To obtain valid hardware counters, run the code on a **native Linux machine** or a Linux VM on hardware that exposes the PMU, and invoke the perf target as root (e.g. sudo make perf).


## Optimization of Matrix Multiplication


## Limitations

On this VPS I couldn’t run matrices with **N=4096** due to resource limits (memory/time). As a result, 4096-sized experiments were skipped; results are reported for N ∈ {1024, 2048}.
