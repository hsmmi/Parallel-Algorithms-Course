# Homework 1 – Matrix Multiplication Profiling

This repo contains a simple CPU‑bound dense matrix multiplication kernel and scripts to collect performance data.

## Goals

- Implement a baseline O(N³) multiplication: `C = A × B` with random inputs.
- Allocate A, B and C as contiguous `new[]` arrays rather than `std::vector`; the course notes point out that contiguous arrays give better data locality.
- Parameterise the problem size with `N` (e.g. 1024, 2048, 4096).
- Automate compilation and runs with the Makefile; average multiple runs (≥3) for each experiment.
- Use `gprof` and `perf` on Linux to profile and understand where time is spent.
- Explore how data type, matrix size, loop order, tiling and unrolling affect performance.

## Building and running

Install a C++ toolchain with `g++`, `gprof` and `perf` (e.g. `sudo apt install build‑essential gprof linux‑tools‑common`).

To compile and run the baseline code:

```sh
# clean previous binaries
make clean

# build and run once with default N=1024
make run

# average three runs and print the mean execution time
make run_avg RUNS=3

# profile with gprof (writes gprof_report_N1024.txt)
make prof

# collect low‑level counters (cycles, instructions, cache misses)
make perf