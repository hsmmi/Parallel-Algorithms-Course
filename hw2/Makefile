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
BIN_DIR := $(dir $(BIN))

# Default target: optimized build
all: $(BIN)

$(BIN): $(SRC) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

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

perf: $(BIN) | $(RESULTS_DIR)
	perf stat -e $(PERF_EVENTS) ./$(BIN) $(INPUT) $(OUTPUT) $(THREADS) $(KSIZE) $(ORDER) $(TILE) $(UNROLL)

# ------------------------
# macOS "perf-like" profiling with xctrace
# ------------------------
# Uses Xcode's Instruments (Time Profiler) via the xctrace CLI.
# Records one run and saves a .trace file in the results directory.

XCTRACE         ?= xcrun xctrace
XCTRACE_TEMPLATE ?= "Time Profiler"
XCTRACE_OUT     ?= $(RESULTS_DIR)/convolve_timeprofiler.trace

mac_profile: $(BIN) | $(RESULTS_DIR)
	$(XCTRACE) record \
	  --template $(XCTRACE_TEMPLATE) \
	  --output $(XCTRACE_OUT) \
	  --launch ./$(BIN) -- \
	    $(INPUT) $(OUTPUT) $(KSIZE)
	@echo "Saved trace to $(XCTRACE_OUT)"
	@echo "Open it with: open $(XCTRACE_OUT)"

# ------------------------
# Run helpers
# ------------------------
# INPUT / OUTPUT can be overridden on the command line, e.g.:
#   make run INPUT=big_2048.png OUTPUT=out_3x3.png

INPUT       ?= input.jpg
RESULTS_DIR ?= results
OUTPUT      ?= $(RESULTS_DIR)/output.png

$(RESULTS_DIR):
	mkdir -p $(RESULTS_DIR)

run: $(BIN) | $(RESULTS_DIR)
	./$(BIN) $(INPUT) $(OUTPUT)

# If your main() later takes more arguments (threads, kernel size,
# loop order, tiling, unrolling, ...), you can reuse these variables
# and this target. Adapt it to match your actual CLI.

THREADS ?= 4
KSIZE   ?= 3
ORDER   ?= 0
TILE    ?= 8
UNROLL  ?= 4

run_exp: $(BIN) | $(RESULTS_DIR)
	./$(BIN) $(INPUT) $(OUTPUT) $(THREADS) $(KSIZE) $(ORDER) $(TILE) $(UNROLL)

run_avg: $(BIN) | $(RESULTS_DIR)
	@runs=""
	@for i in 1 2 3; do \
	  t=$$(./$(BIN) $(INPUT) $(OUTPUT) $(THREADS) $(KSIZE) $(ORDER) $(TILE) $(UNROLL) | awk '/^CONV_TIME/ {print $$2}'); \
	  echo "Run $$i: $$t s"; \
	  runs="$$runs $$t"; \
	done; \
	avg=$$(echo $$runs | awk '{s=0; n=0; for(i=1;i<=NF;i++){s+=$$i;n++} if(n>0) printf "%.6f", s/n}'); \
	echo "Average time (s): $$avg"

# ------------------------
# Variant run helpers
# ------------------------
# These use the long-form CLI:
#   ./bin/convolve_stb input output threads ksize order tile unroll
#
# They assume THREADS=1 and a 2048x2048-style test image. You can
# override INPUT on the command line if needed.

run_base_k3: $(BIN) | $(RESULTS_DIR)
	./$(BIN) $(INPUT) $(RESULTS_DIR)/base_k3.png 1 3 0 0 0

run_base_k15: $(BIN) | $(RESULTS_DIR)
	./$(BIN) $(INPUT) $(RESULTS_DIR)/base_k15.png 1 15 0 0 0

run_order1_k3: $(BIN) | $(RESULTS_DIR)
	./$(BIN) $(INPUT) $(RESULTS_DIR)/order1_k3.png 1 3 1 0 0

run_order2_k3: $(BIN) | $(RESULTS_DIR)
	./$(BIN) $(INPUT) $(RESULTS_DIR)/order2_k3.png 1 3 2 0 0

run_tile8_k15: $(BIN) | $(RESULTS_DIR)
	./$(BIN) $(INPUT) $(RESULTS_DIR)/tile8_k15.png 1 15 0 8 0

run_tile16_k15: $(BIN) | $(RESULTS_DIR)
	./$(BIN) $(INPUT) $(RESULTS_DIR)/tile16_k15.png 1 15 0 16 0

run_unroll4_k15: $(BIN) | $(RESULTS_DIR)
	./$(BIN) $(INPUT) $(RESULTS_DIR)/unroll4_k15.png 1 15 0 0 4

run_unroll8_k15: $(BIN) | $(RESULTS_DIR)
	./$(BIN) $(INPUT) $(RESULTS_DIR)/unroll8_k15.png 1 15 0 0 8

# Run all variants in sequence
run_all: run_base_k3 run_base_k15 run_order1_k3 run_order2_k3 run_tile8_k15 run_tile16_k15 run_unroll4_k15 run_unroll8_k15

# ------------------------
# Cleaning
# ------------------------

clean:
	rm -f $(BIN) gmon.out perf.data

.PHONY: all clean run run_exp gprof_build perf run_avg mac_profile \
        run_base_k3 run_base_k15 run_order1_k3 run_order2_k3 \
        run_tile8_k15 run_tile16_k15 run_unroll4_k15 run_unroll8_k15 run_all
 