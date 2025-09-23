CXX = g++
CXXFLAGS = -O3 -std=c++17

TARGET = merge_sort_demo

all: $(TARGET)

$(TARGET): merge_sort_performance.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(TARGET)
