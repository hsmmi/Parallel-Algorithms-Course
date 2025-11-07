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

## Notes on perf

Collecting hardware events requires access to the CPU’s performance monitoring unit (PMU).  On some virtual machines the counters may be unavailable, in which case perf will report events as `<not supported>`.  In particular, running on a macOS host inside Parallels, VMware or other Mac hypervisors does not virtualise the PMU, so perf cannot measure cycles, instructions or cache misses.  To obtain valid hardware counters, run the code on a **native Linux machine** or a Linux VM on hardware that exposes the PMU, and invoke the perf target as root (e.g. sudo make perf).


## Optimization of Matrix Multiplication
