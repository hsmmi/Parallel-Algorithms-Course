# Homework 1

## Getting Started with Profiling

This assignment is a warm‑up in *measuring and reasoning about performance* for CPU‑bound kernels. You will implement a straightforward dense matrix multiplication (C := A×B) and use profiling tools to understand where time is spent and how changes to code and problem size affect performance.

### Objectives
- Build a baseline \(O(N^3)\) matrix multiplication and a simple correctness check (e.g., checksum).
- Automate builds and runs with a Makefile; parameterize the problem size via `N` (e.g., 1024, 2048, 4096).
- Collect wall‑clock time and basic profiles; interpret function‑level costs and (on Linux) hardware counters.
- Make small, controlled changes (loop order, blocking/tiling, unrolling, data type) and compare results.

### Tools
- **Linux (recommended):** `gprof` (compile with `-pg`) and `perf` (events like `cycles,instructions,cache-misses,L1-dcache-load-misses`).
- **macOS / Apple Silicon (acceptable alternative):** use **gperftools/pprof** instead of `gprof`, and `sample`/Instruments for sampling instead of `perf`. If you use these, *note the substitution* clearly in your report.

### Repository Layout
- `matmul.cpp` — baseline kernel and optional variants controlled via `#define`s.
- `Makefile` — targets like `run`, `gprof` (and optional `perf`); writes logs into `results/`.
- `results/` — profiling outputs (e.g., `gprof_N1024.txt`, `perf_N1024.txt`, `pprof_N1024.txt`).

### Quick Start
- Build & run (default optimization flags):
  ```bash
  make clean && make run N=1024
  ```
- **gprof on Linux:**
  ```bash
  make gprof N=1024
  # output saved under results/gprof_N1024.txt
  ```
- **perf on Linux (after adding a PERF target):**
  ```bash
  make perf N=1024
  # collects cycles,instructions,cache-misses,L1-dcache-load-misses
  ```
- **pprof on macOS (gperftools substitute for gprof):**
  ```bash
  CPUPROFILE=results/pprof_N1024 ./matmul
  pprof --text ./matmul results/pprof_N1024 > results/pprof_N1024.txt
  ```
- **Sampling on macOS (perf substitute):**
  ```bash
  sudo sample ./matmul 10 -file results/sample_N1024.txt
  ```

### Deliverables
- Source code and Makefile.
- Profiling logs (gprof/pprof and perf/sample outputs) inside `results/`.
- A short write‑up (≈1–3 pages) summarizing methodology, key metrics/tables/plots, and 2–3 insights about performance (e.g., effects of size, loop order, locality, and unrolling).

> **Grading emphasizes** reproducibility, clear methodology, and thoughtful analysis — not just raw speed.
