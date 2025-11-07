# Homework 1

## Getting Started with Profiling (macOS setup, multi‑program)

This assignment is a warm‑up in *measuring and reasoning about performance* for a CPU‑bound kernel (dense matrix multiply, `C := A × B`). You will build a baseline, automate runs, and collect timing and profiling data. The provided Makefile is **macOS‑only**, supports **multiple programs** (every `*.cpp` in this folder), builds **per‑N binaries**, and can **repeat runs and average** results.

> On macOS we use **gperftools/pprof** instead of `gprof`, and the built‑in `sample` instead of Linux `perf`.

---

### Objectives
- Build a baseline \(O(N^3)\) matrix multiplication with a simple correctness check (e.g., checksum).
- Automate builds and runs with a Makefile; parameterize the problem size via `N` (e.g., 1024, 2048, 4096).
- **Run each experiment ≥3 times and report the average.**
- Collect profiles and interpret where time is spent; compare effects of loop order, tiling, unrolling, and data type.

---

### Tools (macOS)
- **Compiler:** Apple clang (`c++`).
- **Profiler (gprof substitute):** **gperftools / pprof** — install once:
  ```bash
  brew install gperftools
  ```
- **Sampling profiler (perf substitute):** built‑in `sample` or Instruments.

---

### Repository layout
- Multiple single‑file programs: any `*.cpp` is treated as a separate program (e.g., `matmul.cpp`, `matrix_int.cpp`, `matrix_double.cpp`, …).
- `Makefile` — multi‑program, **per‑N binaries**, and **averaging** targets:
  - Single‑program targets (use `PROG=<name>`): `build`, `run`, `run_avg`, `profile`, `pprof`, `pprof_avg`, `sample`
  - All‑program targets: `all_build`, `all_run`, `all_run_avg`, `all_pprof`, `all_pprof_avg`, `all_sample`
- Results tree (tool‑separated, per‑program):
  - `results/run/<prog>/` — timings & averages
  - `results/gprof/<prog>/` — **pprof** reports (macOS substitute for gprof)
  - `results/perf/<prog>/` — **sample** reports (macOS substitute for perf)

> Binaries are named per size (e.g., `matrix_int_N1024`). macOS may create `*.dSYM/` bundles for symbols (safe to delete or keep for better profiles).

---

### Quick start — single program
- List detected programs (from `*.cpp`):
  ```bash
  make list
  ```
- **Build & run once** (defaults: `PROG=matmul`, `N=1024`):
  ```bash
  make clean && make run PROG=matmul N=1024
  ```
- **Run 3× and compute the average** (default `RUNS=3`, configurable):
  ```bash
  make run_avg PROG=matrix_int N=1024            # results/run/matrix_int/avg_N1024.txt
  make run_avg PROG=matrix_double N=2048 RUNS=5  # 5 repetitions
  ```
- **pprof: profile once** (requires gperftools):
  ```bash
  make pprof PROG=matrix_int N=1024              # results/gprof/matrix_int/pprof_N1024.txt
  ```
- **pprof: run 3×, average and merge profiles**
  ```bash
  make pprof_avg PROG=matrix_double N=2048 RUNS=3  # avg + merged -> results/gprof/matrix_double/
  ```
- **Sampling (perf‑like) for 10s**
  ```bash
  make sample PROG=matrix_int N=1024 SAMPLE_SECS=10  # results/perf/matrix_int/sample_N1024.txt
  ```

---

### Run the whole suite (all programs)
- Build everything for a given size:
  ```bash
  make all_build N=1024
  ```
- Run once for all programs:
  ```bash
  make all_run N=1024
  ```
- Average timings for all programs:
  ```bash
  make all_run_avg N=1024 RUNS=3
  ```
- pprof for all programs (once / averaged):
  ```bash
  make all_pprof N=1024
  make all_pprof_avg N=1024 RUNS=3
  ```
- sample for all programs:
  ```bash
  make all_sample N=1024 SAMPLE_SECS=10
  ```

---

### Notes on parameters & output parsing
- `N` controls matrix size at **compile time** (binaries are named `<prog>_N$(N)`). Changing `N` builds a new binary.
- `PROG` selects which program to run; default is `matmul`. Examples: `matrix_int`, `matrix_double`.
- `RUNS` controls how many repetitions `run_avg`/`pprof_avg` perform.
- Timing is parsed from the program’s line: `Execution time: <seconds> seconds`. Keep that exact format in each `*.cpp`.
- Folder mapping mirrors the original Linux homework wording: `gprof/` (pprof on macOS) and `perf/` (macOS `sample`).

---

### Deliverables
- Source code and Makefile.
- Logs/reports in `results/`, organized per tool and per program:
  - `results/run/<prog>/avg_N*.txt` (averaged runtime) plus per‑run logs.
  - `results/gprof/<prog>/pprof_N*.txt` (single run) and `.../avg_N*.txt` + merged profile.
  - Optional `results/perf/<prog>/sample_N*.txt` snapshots.
- A short write‑up (≈1–3 pages) that:
  1. Describes methodology and automation.
  2. Presents **averaged** results (≥3 runs) in clear tables/plots.
  3. Explains key insights (effects of size N, loop order, locality from tiling, unrolling, data type).

> **Grading emphasizes** reproducibility, averaged results, and thoughtful analysis — not just raw speed.