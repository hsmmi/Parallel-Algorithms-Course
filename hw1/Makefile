# ============================
#   Makefile — macOS only, multi-program, per‑N, averaged, tool‑separated results
#   Note: "gprof" folder stores pprof outputs on macOS; "perf" folder stores macOS 'sample' outputs.
# ============================

# Compiler (Apple clang by default)
CXX ?= c++

# Common compile flags (no -march on Apple Silicon)
CXXFLAGS := -O3 -funroll-loops -ffast-math -Wall -g
LDFLAGS  :=

# Homebrew include/lib (if present)
HOMEBREW_PREFIX := $(shell brew --prefix 2>/dev/null)
ifneq ($(HOMEBREW_PREFIX),)
  CXXFLAGS += -I$(HOMEBREW_PREFIX)/include
  LDFLAGS  += -L$(HOMEBREW_PREFIX)/lib
endif

# ---- Programs & parameters ---------------------------------------------------
# Grab every .cpp in this folder (so newly added files are auto‑picked up)
SOURCES := $(wildcard *.cpp)
PROGS   := $(SOURCES:.cpp=)

# Choose which program to act on (default: matmul)
PROG ?= matmul

# Problem size, repetitions, sample seconds
N           ?= 1024
RUNS        ?= 3
SAMPLE_SECS ?= 10

# Binaries are per‑program, per‑N
BIN      := $(PROG)_N$(N)
PROF_BIN := $(PROG)_prof_N$(N)

# Results directories (tool‑separated, per‑program)
RESULTS_DIR  := results
RUN_DIR      := $(RESULTS_DIR)/run/$(PROG)
GPROF_DIR    := $(RESULTS_DIR)/gprof/$(PROG)   # holds pprof outputs on macOS
PERF_DIR     := $(RESULTS_DIR)/perf/$(PROG)    # holds 'sample' outputs

# Ensure base result dirs exist
$(shell mkdir -p $(RESULTS_DIR) $(RESULTS_DIR)/run $(RESULTS_DIR)/gprof $(RESULTS_DIR)/perf)

.PHONY: help list all all_build all_run all_run_avg all_pprof all_pprof_avg all_sample \
        build run run_avg profile pprof pprof_avg sample clean

# ---- Build rules -------------------------------------------------------------
# Pattern: build per‑N binary from a single‑file program
%_N$(N): %.cpp
	$(CXX) $(CXXFLAGS) -DN=$(N) -o $@ $< $(LDFLAGS)
	@echo "Built $@ (N=$(N))"

# Pattern: build profiled (gperftools) per‑N binary
%_prof_N$(N): %.cpp
	$(CXX) $(CXXFLAGS) -DN=$(N) -o $@ $< $(LDFLAGS) -lprofiler
	@echo "Built $@ (N=$(N)) with -lprofiler"

# Convenience build for current PROG
build: $(BIN)

# Build every program for this N
all_build:
	@for p in $(PROGS); do \
	  $(MAKE) --no-print-directory BIN=$${p}_N$(N) PROG=$$p build N=$(N); \
	done

# Default make target
all: build

# ---- Run (single) ------------------------------------------------------------
run: $(BIN)
	@mkdir -p $(RUN_DIR)
	@echo "Running $(BIN) (PROG=$(PROG), N=$(N))..."
	@time ./$(BIN)

# ---- Run (average over RUNS) -------------------------------------------------
# Parses line:  "Execution time: <seconds> seconds"
run_avg: $(BIN)
	@mkdir -p $(RUN_DIR)
	@echo "Running $(BIN) $(RUNS)x and averaging..."
	@rm -f $(RUN_DIR)/times_N$(N).tmp
	@i=1; while [ $$i -le $(RUNS) ]; do \
	  out=$$(./$(BIN)); \
	  echo "[run $$i]"       >> $(RUN_DIR)/runs_N$(N).log; \
	  echo "$$out"           >> $(RUN_DIR)/runs_N$(N).log; \
	  echo "$$out" | awk '/Execution time:/ {print $$3}' >> $(RUN_DIR)/times_N$(N).tmp; \
	  i=$$((i+1)); \
	done
	@avg=$$(awk '{s+=$$1} END{if(NR>0) printf "%.6f", s/NR}' $(RUN_DIR)/times_N$(N).tmp); \
	  { echo "PROG=$(PROG), N=$(N), RUNS=$(RUNS)"; \
	    echo -n "Times (s): "; tr '\n' ' ' < $(RUN_DIR)/times_N$(N).tmp; echo; \
	    echo "Average (s): $$avg"; } | tee $(RUN_DIR)/avg_N$(N).txt >/dev/null; \
	echo "Saved average to $(RUN_DIR)/avg_N$(N).txt"

# Run every program and average
all_run_avg:
	@for p in $(PROGS); do \
	  $(MAKE) --no-print-directory PROG=$$p N=$(N) RUNS=$(RUNS) run_avg; \
	done

# ---- pprof (stored under results/gprof/<prog>) --------------------------------
profile: $(PROF_BIN)

pprof: profile
	@mkdir -p $(GPROF_DIR)
	@if ! command -v pprof >/dev/null 2>&1; then \
	  echo "ERROR: 'pprof' not found. Install with: brew install gperftools"; exit 1; \
	fi
	@echo "Profiling $(PROF_BIN) with gperftools (pprof) ..."
	@CPUPROFILE=$(GPROF_DIR)/pprof_N$(N).out ./$(PROF_BIN)
	@pprof --text ./$(PROF_BIN) $(GPROF_DIR)/pprof_N$(N).out > $(GPROF_DIR)/pprof_N$(N).txt
	@echo "pprof text report -> $(GPROF_DIR)/pprof_N$(N).txt"

pprof_avg: profile
	@mkdir -p $(GPROF_DIR)
	@if ! command -v pprof >/dev/null 2>&1; then \
	  echo "ERROR: 'pprof' not found. Install with: brew install gperftools"; exit 1; \
	fi
	@rm -f $(GPROF_DIR)/pprof_N$(N)_run*.out $(GPROF_DIR)/times_N$(N).tmp
	@i=1; while [ $$i -le $(RUNS) ]; do \
	  export CPUPROFILE=$(GPROF_DIR)/pprof_N$(N)_run$$i.out; \
	  out=$$(./$(PROF_BIN)); \
	  echo "[pprof run $$i]" >> $(GPROF_DIR)/runs_N$(N).log; \
	  echo "$$out"           >> $(GPROF_DIR)/runs_N$(N).log; \
	  echo "$$out" | awk '/Execution time:/ {print $$3}' >> $(GPROF_DIR)/times_N$(N).tmp; \
	  i=$$((i+1)); \
	done
	@pprof --text ./$(PROF_BIN) $(GPROF_DIR)/pprof_N$(N)_run*.out > $(GPROF_DIR)/pprof_N$(N)_merged.txt
	@avg=$$(awk '{s+=$$1} END{if(NR>0) printf "%.6f", s/NR}' $(GPROF_DIR)/times_N$(N).tmp); \
	  { echo "PROG=$(PROG), N=$(N), RUNS=$(RUNS)"; \
	    echo -n "Times (s): "; tr '\n' ' ' < $(GPROF_DIR)/times_N$(N).tmp; echo; \
	    echo "Average (s): $$avg"; \
	    echo "Merged profile: $(GPROF_DIR)/pprof_N$(N)_merged.txt"; } \
	    | tee $(GPROF_DIR)/avg_N$(N).txt >/dev/null; \
	echo "Saved average to $(GPROF_DIR)/avg_N$(N).txt"

# pprof for every program
all_pprof:
	@for p in $(PROGS); do \
	  $(MAKE) --no-print-directory PROG=$$p N=$(N) pprof; \
	done
all_pprof_avg:
	@for p in $(PROGS); do \
	  $(MAKE) --no-print-directory PROG=$$p N=$(N) RUNS=$(RUNS) pprof_avg; \
	done

# ---- 'perf' substitute: macOS sample (stored under results/perf/<prog>) -------
sample: $(BIN)
	@mkdir -p $(PERF_DIR)
	@echo "Sampling ./$(BIN) for $(SAMPLE_SECS)s ..."
	@/usr/bin/sample ./$(BIN) $(SAMPLE_SECS) -file $(PERF_DIR)/sample_N$(N).txt
	@echo "sample report -> $(PERF_DIR)/sample_N$(N).txt"

# sample every program
all_sample:
	@for p in $(PROGS); do \
	  $(MAKE) --no-print-directory PROG=$$p N=$(N) SAMPLE_SECS=$(SAMPLE_SECS) sample; \
	done

# ---- Run once for all programs (no avg) -------------------------------------
all_run:
	@for p in $(PROGS); do \
	  $(MAKE) --no-print-directory PROG=$$p N=$(N) run; \
	done

# ---- Utility targets ---------------------------------------------------------
list:
	@echo "Programs detected (from *.cpp):"
	@for p in $(PROGS); do echo "  - $$p"; done

help:
	@echo "Usage: make <target> [PROG=name] [N=1024] [RUNS=3] [SAMPLE_SECS=10]"
	@echo "Targets (single program, use PROG=...):"
	@echo "  build        -> build per‑N binary: $(BIN)"
	@echo "  run          -> run once (writes to results/run/$(PROG)/)"
	@echo "  run_avg      -> run $(RUNS)x and average (results/run/$(PROG)/avg_N$(N).txt)"
	@echo "  profile      -> build profiled binary: $(PROF_BIN)"
	@echo "  pprof        -> pprof once (results/gprof/$(PROG)/pprof_N$(N).txt)"
	@echo "  pprof_avg    -> pprof $(RUNS)x + merged (results/gprof/$(PROG)/...)"
	@echo "  sample       -> macOS 'sample' (results/perf/$(PROG)/sample_N$(N).txt)"
	@echo "Targets (all programs detected):"
	@echo "  all_build, all_run, all_run_avg, all_pprof, all_pprof_avg, all_sample"
	@echo "Other: list, help, clean"

clean:
	rm -f matmul matmul_prof *_N* *_prof_N* gmon.out
	rm -rf *.dSYM
	@echo "Cleaned binaries. Results remain under '$(RESULTS_DIR)/'"