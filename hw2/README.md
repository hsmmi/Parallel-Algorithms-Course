

# HW2 – Parallel 2D Convolution (Pthreads)

This repository contains my solution-in-progress for **Homework 2: Parallel 2D Convolution using Pthreads** for the *Parallel Algorithms* course.

The goal of this assignment is to implement and study different optimizations for 2D image convolution:

- Multi-threading with **POSIX threads (pthreads)**
- Varying **loop orders**
- **Tiling (blocking)** in the spatial dimensions
- **Loop unrolling**
- Experimenting with different **kernel sizes** (e.g., 3×3 vs 31×31)
- Collecting and analyzing performance data with **profiling tools**

> **Note:** I am working on an Apple Silicon (M1) Mac. Linux `perf` is not natively available here, so I will either run my code in a Linux environment when I need true `perf` data, or use Apple’s profiling tools (Instruments / `xctrace`, `powermetrics`, etc.) as alternatives when appropriate.

---

## Files

- `convolve_stb.c`  
  Main C source file for the convolution program (uses stb_image / stb_image_write for I/O in the original template).

- `Makefile`  
  Build and run helper for this homework. Currently supports:
  - Building an optimized binary with `-O2` and debug symbols
  - Building a `gprof`-instrumented version
  - Simple `run` and `run_exp` targets to execute the program with different arguments
  - A `perf` target intended for Linux environments (can be ignored on macOS)

- `HW2 - Parallel 2D Convolution using Pthreads.pdf`  
  The homework description (not tracked in this README, but referenced while implementing).

---

## Building

From the `hw2` directory:

```bash
make          # Build the default optimized binary (convolve_stb)
```

Compiler flags used by default:

```bash
CFLAGS = -O2 -Wall -Wextra -std=c11 -g
LDFLAGS = -lm -lpthread
```

To clean old builds:

```bash
make clean
```

---

## Running

The current `Makefile` assumes a basic interface like:

```bash
./convolve_stb input.jpg output.png
```

You can run this via:

```bash
make run
```

The input and output paths can be overridden on the command line:

```bash
make run INPUT=big_2048.jpg OUTPUT=out_3x3.png
```

There is also an experimental target intended for when `main()` is extended to accept more parameters (threads, kernel size, loop order, tiling, unrolling, etc.):

```bash
make run_exp \
  INPUT=big_2048.jpg \
  OUTPUT=out_31x31_t8_u4.png \
  THREADS=8 KSIZE=31 ORDER=0 TILE=8 UNROLL=4
```

This expands to:

```bash
./convolve_stb INPUT OUTPUT THREADS KSIZE ORDER TILE UNROLL
```

At the current stage, this is a placeholder and will be kept in sync with the actual `main()` signature as the implementation evolves.

---

## Profiling

### gprof

There is a dedicated target to build a `gprof`-instrumented binary:

```bash
make gprof_build
```

This rebuilds `convolve_stb` with `-pg`. Example usage:

```bash
./convolve_stb input.jpg output.png
gprof ./convolve_stb gmon.out > gprof.txt
```

The generated `gprof.txt` will be used in the report to discuss which functions dominate the runtime and how different optimizations affect the call graph.

### perf (Linux)

On a Linux system with `perf` installed, the `perf` target can be used:

```bash
make perf INPUT=big_2048.jpg OUTPUT=out_perf.png
```

which runs something like:

```bash
perf stat -e cycles,instructions,cache-misses,L1-dcache-load-misses \
  ./convolve_stb big_2048.jpg out_perf.png
```

On macOS/M1, I will instead rely on:

- **Xcode Instruments / `xctrace`** (Time Profiler, CPU Counters) for detailed profiling
- **`powermetrics`** or tools built on top of it (e.g., asitop/fluidtop) for monitoring hardware usage

These will serve as the “similar tools” to perf mentioned in the assignment when I am not on a Linux machine.

---

## Status / TODO (so far)

- [x] Set up basic `Makefile` with:
  - Optimized build (`-O2`)
  - gprof build
  - run helpers and placeholders for experimental parameters
  - optional perf target for Linux
- [ ] Finalize and test multi-threaded convolution with pthreads
- [ ] Implement and experiment with different loop orders
- [ ] Implement tiling (blocking) versions
- [ ] Implement loop-unrolled versions (e.g., unroll factor 4 and 8)
- [ ] Collect profiling data (gprof + perf/alternatives) for different configurations
- [ ] Write the final report with speedup plots and analysis

This README describes the project **up to the current implementation stage** and will be updated as new features and results are added.