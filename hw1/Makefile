# ============================
#   Makefile for HW1 Profiling
# ============================

CXX = g++
CXXFLAGS = -O3 -march=native -funroll-loops -ffast-math -Wall
SRC = matmul.cpp
TARGET = matmul
PROF_EXE = matmul_prof
RESULTS_DIR = results

# Default matrix size (can be overridden: make run N=512)
N ?= 1024

# Create results directory if not exists
$(shell mkdir -p $(RESULTS_DIR))

# -------------------------------------
# 1. Compile baseline executable
# -------------------------------------
all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -DN=$(N) -o $@ $^

# -------------------------------------
# 2. Run the baseline
# -------------------------------------
run: $(TARGET)
	@echo "Running matrix multiplication (N=$(N))..."
	@time ./$(TARGET)

# -------------------------------------
# 3. Compile with profiling enabled
# -------------------------------------
profile: $(SRC)
	$(CXX) $(CXXFLAGS) -pg -DN=$(N) -o $(PROF_EXE) $^

# -------------------------------------
# 4. Run and collect gprof results
# -------------------------------------
gprof: profile
	@echo "Running with gprof (N=$(N))..."
	./$(PROF_EXE)
	@gprof $(PROF_EXE) gmon.out > $(RESULTS_DIR)/gprof_N$(N).txt
	@echo "gprof results saved to $(RESULTS_DIR)/gprof_N$(N).txt"

# -------------------------------------
# 5. Clean build files
# -------------------------------------
clean:
	rm -f $(TARGET) $(PROF_EXE) gmon.out