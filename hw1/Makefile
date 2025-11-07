# Compiler and flags
CXX = g++
CXXFLAGS = -O3 -funroll-loops -ffast-math -Wall -march=native

# Directories
SRC_DIR := src
BIN_DIR := bin

# Program name (without .cpp extension)
PROG ?= matmul_int

# Matrix dimension and runs
N ?= 1024
RUNS ?= 3

# Ensure bin directory exists
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# Derived binary names (per N), now living in bin/
BIN      := $(BIN_DIR)/$(PROG)_N$(N)
PROF_BIN := $(BIN_DIR)/$(PROG)_gprof_N$(N)

# Build baseline binary
$(BIN): $(SRC_DIR)/$(PROG).cpp
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -DN=$(N) -o $@ $<

# Build gprof‑instrumented binary
$(PROF_BIN): $(SRC_DIR)/$(PROG).cpp
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -pg -DN=$(N) -o $@ $<
	
# Default target: build baseline
build: $(BIN)

# Run baseline once
run: build
	./$(BIN)

# Run baseline RUNS times and average
run_avg: build
	@rm -f .times.tmp
	@i=1; while [ $$i -le $(RUNS) ]; do \
		out=$$(./$(BIN)); \
		echo "$$out"; \
		echo "$$out" | awk '/Execution time:/ {print $$3}' >> .times.tmp; \
		i=$$((i+1)); \
	done; \
	avg=$$(awk '{s+=$$1} END{printf \"%.6f\", s/NR}' .times.tmp); \
	echo "Average execution time: $$avg seconds"

# Generate a gprof report
prof: $(PROF_BIN)
	@rm -f gmon.out
	./$(PROF_BIN)
	gprof $(PROF_BIN) gmon.out > gprof_report_N$(N).txt
	@echo "gprof report saved to gprof_report_N$(N).txt"

# Perf metrics
perf: $(BIN)
	perf stat -e cycles,instructions,cache-misses,cache-references,L1-dcache-load-misses ./$(BIN)

# Clean up generated files
clean:
	rm -rf $(BIN_DIR) gmon.out gprof_report_N*.txt .times.tmp