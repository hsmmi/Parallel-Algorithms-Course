# Compiler and flags
CXX = g++
CXXFLAGS = -O3 -funroll-loops -ffast-math -Wall -march=native

# Program name (source file without extension)
PROG ?= matmul_int

# Matrix dimension (NxN); override on the command line, e.g. `make N=2048`
N ?= 1024

# Number of runs when averaging execution time
RUNS ?= 3

# Derived binary names (per N)
BIN      := $(PROG)_N$(N)
PROF_BIN := $(PROG)_gprof_N$(N)

# Build baseline binary with chosen N
$(BIN): $(PROG).cpp
	$(CXX) $(CXXFLAGS) -DN=$(N) -o $@ $<

# Build gprof‑instrumented binary
$(PROF_BIN): $(PROG).cpp
	$(CXX) $(CXXFLAGS) -pg -DN=$(N) -o $@ $<

# Default target: build baseline
build: $(BIN)

# Run baseline once
run: build
	./$(BIN)

# Run baseline RUNS times and compute average execution time
run_avg: build
	@rm -f .times.tmp
	@i=1; while [ $$i -le $(RUNS) ]; do \
		out=$$(./$(BIN)); \
		echo "$$out"; \
		echo "$$out" | awk '/Execution time:/ {print $$3}' >> .times.tmp; \
		i=$$((i+1)); \
	done; \
	avg=$$(awk '{s+=$$1} END{printf "%.6f", s/NR}' .times.tmp); \
	echo "Average execution time: $$avg seconds"

# Generate a gprof report (gmon.out + text report)
prof: $(PROF_BIN)
	@rm -f gmon.out
	./$(PROF_BIN)
	gprof $(PROF_BIN) gmon.out > gprof_report_N$(N).txt
	@echo "gprof report saved to gprof_report_N$(N).txt"

# Use Linux perf stat to gather low‑level metrics
perf: $(BIN)
	perf stat -e cycles,instructions,cache-misses,cache-references,L1-dcache-load-misses ./$(BIN)

# Clean all generated binaries and temporary files
clean:
	rm -f $(BIN) $(PROF_BIN) gmon.out gprof_report_N*.txt .times.tmp