# Homework 1 – Matrix Multiplication Profiling

This repo contains a simple CPU‑bound dense matrix multiplication kernel and scripts to collect performance data.

## Goals

- Implement a baseline O(N³) multiplication: `C = A × B` with random inputs.
- Parameterise the problem size with `N` (e.g. 1024, 2048, 4096).
- Automate compilation and runs with the Makefile; average multiple runs (≥3) for each experiment.
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

# average three runs and print the mean execution time
make run_avg RUNS=3

# profile with gprof (writes gprof_report_N1024.txt)
make gprof

# collect low‑level counters (cycles, instructions, cache misses)
make perf
```

## Optimization of Matrix Multiplication
