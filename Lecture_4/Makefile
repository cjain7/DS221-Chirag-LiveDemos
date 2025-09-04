CXX = g++
CXXFLAGS = -O3 -std=c++17

TARGET = heapsort_demo

all: $(TARGET)

$(TARGET): heapsort_vs_cppsort.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(TARGET)
