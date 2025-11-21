# ========================
# HW2 - Parallel 2D Convolution (Pthreads)
# Makefile
# ========================

# Compiler and basic flags
CC      ?= gcc
CFLAGS  ?= -O2 -Wall -Wextra -std=c11 -g
LDFLAGS ?= -lm -lpthread

# Source and binary
SRC     := src/convolve_stb.c
BIN     := bin/convolve_stb

# Default target: optimized build
all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# ------------------------
# Profiling build (gprof)
# ------------------------
# This target rebuilds the binary with -pg enabled so you can run gprof.

GPROF_FLAGS := -pg

gprof_build: CFLAGS += $(GPROF_FLAGS)

gprof_build: LDFLAGS += $(GPROF_FLAGS)

gprof_build: clean $(BIN)

# Example usage (after building with gprof_build):
#   ./convolve_stb input.jpg out.png
#   gprof ./convolve_stb gmon.out > gprof.txt

# ------------------------
# Perf-style profiling (Linux only)
# ------------------------
# If you run this on a Linux machine with perf installed, this target
# automates a typical perf stat invocation. On macOS you can ignore it
# and instead use Instruments / xctrace.

PERF_EVENTS ?= cycles,instructions,cache-misses,L1-dcache-load-misses

perf: $(BIN)
	perf stat -e $(PERF_EVENTS) ./$(BIN) $(INPUT) $(OUTPUT)

# ------------------------
# Run helpers
# ------------------------
# INPUT / OUTPUT can be overridden on the command line, e.g.:
#   make run INPUT=big_2048.png OUTPUT=out_3x3.png

INPUT  ?= input.jpg
OUTPUT ?= output.png

run: $(BIN)
	./$(BIN) $(INPUT) $(OUTPUT)

# If your main() later takes more arguments (threads, kernel size,
# loop order, tiling, unrolling, ...), you can reuse these variables
# and this target. Adapt it to match your actual CLI.

THREADS ?= 4
KSIZE   ?= 3
ORDER   ?= 0
TILE    ?= 8
UNROLL  ?= 4

run_exp: $(BIN)
	./$(BIN) $(INPUT) $(OUTPUT) $(THREADS) $(KSIZE) $(ORDER) $(TILE) $(UNROLL)

# ------------------------
# Cleaning
# ------------------------

clean:
	rm -f $(BIN) gmon.out perf.data

.PHONY: all clean run run_exp gprof_build perf
