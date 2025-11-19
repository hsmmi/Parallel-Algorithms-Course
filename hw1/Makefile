# Compiler and flags
CXX = g++
CXXFLAGS = -O3 -funroll-loops -ffast-math -Wall -march=native

# Directories
SRC_DIR := src
BIN_DIR := bin
RESULTS_DIR := results
# Program-specific result directories
GPROF_DIR   := $(RESULTS_DIR)/gprof/$(PROG)
PERF_DIR    := $(RESULTS_DIR)/perf/$(PROG)

# Program name (without .cpp extension)
PROG ?= matmul_int

# Matrix dimension and runs
N ?= 1024
RUNS ?= 3
TILE ?= 32
TAG  ?=
UNROLL ?= 4

# Problem sizes for the part1 target
SIZES := 1024 2048 4096
# Experiment matrix for part1: items are prog,size,runs,tools
EXPERIMENTS_P1 := \
	matmul_double,1024,3,all \
	matmul_int,1024,3,all \
	matmul_int,2048,3,all \
	matmul_int,4096,3,all

# Experiment matrix for part3 (loop order permutations on N=4096)
EXPERIMENTS_P3 := \
	matmul_ijk,4096,3,all \
	matmul_ikj,4096,3,all \
	matmul_jik,4096,3,all \
	matmul_jki,4096,3,all \
	matmul_kij,4096,3,all \
	matmul_kji,4096,3,all

# Tiling sweep (part4)
TILE_SIZES := 16 32 64
P2_SIZES   := 1024 2048

# Unrolling sweep (part5)
UNROLLS  := 4 8
P4_SIZES := 1024 2048

# List of all source files in src/ and derived program names
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
PROGS := $(patsubst $(SRC_DIR)/%.cpp,%,$(SRC_FILES))

# Ensure directories exist
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)
$(GPROF_DIR):
	@mkdir -p $(GPROF_DIR)
$(PERF_DIR):
	@mkdir -p $(PERF_DIR)

# Derived binary names (per N), now living in bin/
BIN      := $(BIN_DIR)/$(PROG)_N$(N)
PROF_BIN := $(BIN_DIR)/$(PROG)_gprof_N$(N)

# Build baseline binary
$(BIN): $(SRC_DIR)/$(PROG).cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -DN=$(N) -DTILE=$(TILE) -DUNROLL=$(UNROLL) -o $@ $<

# Build gprof-instrumented binary
$(PROF_BIN): $(SRC_DIR)/$(PROG).cpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -pg -DN=$(N) -DTILE=$(TILE) -DUNROLL=$(UNROLL) -o $@ $<
	
# Default target: build baseline
build: $(BIN)

# Run baseline once
run: build
	./$(BIN)

# Generate gprof reports across RUNS runs and compute average execution time
gprof: $(PROF_BIN) | $(GPROF_DIR)
	# remove any prior run files and temporary times
	@rm -f $(GPROF_DIR)/gmon$(TAG)_N$(N)_run*.out $(GPROF_DIR)/gprof$(TAG)_N$(N)_run*.txt .gprof_times.tmp
	@i=1; while [ $$i -le $(RUNS) ]; do \
		rm -f gmon.out; \
		out=$$(./$(PROF_BIN)); \
		echo "$$out"; \
		echo "$$out" | awk '/Execution time:/ {print $$3}' >> .gprof_times.tmp; \
		# save the generated gmon.out for this run \
		mv gmon.out $(GPROF_DIR)/gmon$(TAG)_N$(N)_run$$i.out; \
		# produce a gprof text report for this run \
		gprof $(PROF_BIN) $(GPROF_DIR)/gmon$(TAG)_N$(N)_run$$i.out > $(GPROF_DIR)/gprof$(TAG)_N$(N)_run$$i.txt; \
		i=$$((i+1)); \
	done; \
	# compute average execution time from collected times \
	avg=$$(awk '{s+=$$1} END{printf "%.6f", s/NR}' .gprof_times.tmp); \
	echo "PROG=$(PROG), N=$(N), RUNS=$(RUNS), TILE=$(TILE)" > $(GPROF_DIR)/avg$(TAG)_N$(N).txt; \
	echo "Average execution time: $$avg seconds" >> $(GPROF_DIR)/avg$(TAG)_N$(N).txt
	@echo "gprof reports saved to $(GPROF_DIR)/gprof$(TAG)_N$(N)_run*.txt and average to $(GPROF_DIR)/avg$(TAG)_N$(N).txt"

# Perf metrics
perf: $(BIN) | $(PERF_DIR)
	sudo perf stat -r $(RUNS) -e cycles,instructions,cache-misses,cache-references,L1-dcache-load-misses \
		-o $(PERF_DIR)/perf$(TAG)_N$(N).txt ./$(BIN)
	@echo "perf stats saved to $(PERF_DIR)/perf$(TAG)_N$(N).txt"

# Loop over all programs and build
all_build:
	@for p in $(PROGS); do \
		$(MAKE) PROG=$$p N=$(N) build; \
	done

# Loop over all programs and run once
all_run: all_build
	@for p in $(PROGS); do \
		$(MAKE) PROG=$$p N=$(N) run; \
	done

# Loop over all programs and gprof
all_gprof: all_build
	@for p in $(PROGS); do \
		$(MAKE) PROG=$$p N=$(N) gprof; \
	done

# Loop over all programs and perf
all_perf: all_build
	@for p in $(PROGS); do \
		$(MAKE) PROG=$$p N=$(N) RUNS=$(RUNS) perf; \
	done

# Run all experiments_P1 across predefined problem sizes (part1)
part1: clean
	@set -e; \
	for item in $(EXPERIMENTS_P1); do \
		IFS=','; set -- $$item; \
		prog=$$1; n=$$2; runs=$$3; tools=$$4; \
		echo "=== Running $$prog N=$$n RUNS=$$runs TOOLS=$$tools ==="; \
		$(MAKE) --no-print-directory PROG=$$prog N=$$n build; \
		$(MAKE) --no-print-directory PROG=$$prog N=$$n run; \
		if [ "$$tools" = "all" ] || [ "$$tools" = "gprof" ]; then \
			$(MAKE) --no-print-directory PROG=$$prog N=$$n RUNS=$$runs gprof; \
		fi; \
		if [ "$$tools" = "all" ] || [ "$$tools" = "perf" ]; then \
			$(MAKE) --no-print-directory PROG=$$prog N=$$n RUNS=$$runs perf; \
		fi; \
	done

# Run all experiments_P3 (part3: loop order permutations)
part3: clean
	@set -e; \
	for item in $(EXPERIMENTS_P3); do \
		IFS=','; set -- $$item; \
		prog=$$1; n=$$2; runs=$$3; tools=$$4; \
		echo "=== Running $$prog N=$$n RUNS=$$runs TOOLS=$$tools ==="; \
		$(MAKE) --no-print-directory PROG=$$prog N=$$n build; \
		$(MAKE) --no-print-directory PROG=$$prog N=$$n run; \
		if [ "$$tools" = "all" ] || [ "$$tools" = "gprof" ]; then \
			$(MAKE) --no-print-directory PROG=$$prog N=$$n RUNS=$$runs gprof; \
		fi; \
		if [ "$$tools" = "all" ] || [ "$$tools" = "perf" ]; then \
			$(MAKE) --no-print-directory PROG=$$prog N=$$n RUNS=$$runs perf; \
		fi; \
	done

# Run tiling experiments (part4)
part4: clean
	@set -e; \
	for n in $(P2_SIZES); do \
		for t in $(TILE_SIZES); do \
			echo "=== matmul_tilling N=$$n TILE=$$t RUNS=$(RUNS) ==="; \
			$(MAKE) --no-print-directory PROG=matmul_tilling N=$$n TILE=$$t TAG=_tile$$t build; \
			$(MAKE) --no-print-directory PROG=matmul_tilling N=$$n TILE=$$t TAG=_tile$$t run; \
			$(MAKE) --no-print-directory PROG=matmul_tilling N=$$n TILE=$$t TAG=_tile$$t RUNS=$(RUNS) gprof; \
			$(MAKE) --no-print-directory PROG=matmul_tilling N=$$n TILE=$$t TAG=_tile$$t RUNS=$(RUNS) perf; \
		done; \
	done

# Run unrolling experiments (part5)
part5: clean
	@set -e; \
	for n in $(P4_SIZES); do \
		for u in $(UNROLLS); do \
			echo "=== matmul_unrolling N=$$n UNROLL=$$u RUNS=$(RUNS) ==="; \
			$(MAKE) --no-print-directory PROG=matmul_unrolling N=$$n UNROLL=$$u TAG=_unroll$$u build; \
			$(MAKE) --no-print-directory PROG=matmul_unrolling N=$$n UNROLL=$$u TAG=_unroll$$u run; \
			$(MAKE) --no-print-directory PROG=matmul_unrolling N=$$n UNROLL=$$u TAG=_unroll$$u RUNS=$(RUNS) gprof; \
			$(MAKE) --no-print-directory PROG=matmul_unrolling N=$$n UNROLL=$$u TAG=_unroll$$u RUNS=$(RUNS) perf; \
		done; \
	done

# Clean up generated files
clean:
	rm -rf $(BIN_DIR) gmon.out gprof_report_N*.txt .times.tmp $(RESULTS_DIR)/run

.PHONY: build run gprof perf clean all_build all_run all_gprof all_perf part1 part3 part4 part5
