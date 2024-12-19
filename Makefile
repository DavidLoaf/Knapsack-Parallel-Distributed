ifdef USE_INT
MACRO = -DUSE_INT
endif

# Compiler setup
CXX = g++
MPICXX = mpic++
CXXFLAGS = -std=c++14 -O3 $(MACRO) -g

COMMON = core/utils.h core/cxxopts.h core/get_time.h 
SERIAL = serial/knapsack_serial
PARALLEL = parallel/knapsack_parallel
DISTRIBUTED = distributed/knapsack_distributed
ALL = $(SERIAL) $(PARALLEL) $(DISTRIBUTED)

all: $(ALL)

$(SERIAL): serial/%: serial/%.cpp
	$(CXX) $(CXXFLAGS) -o build/$(*F) $<

$(PARALLEL): parallel/%: parallel/%.cpp
	$(CXX) $(CXXFLAGS) -o build/$(*F) $< -lpthread

$(DISTRIBUTED): distributed/%: distributed/%.cpp
	$(MPICXX) $(CXXFLAGS) -o build/$(*F) $<

.PHONY: clean

clean:
	rm -f build/* serial/*.o serial/*.obj parallel/*.o parallel/*.obj distributed/*.o distributed/*.obj $(ALL)
