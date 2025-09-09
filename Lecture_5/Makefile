CXX = g++
CXXFLAGS = -O3 -std=c++17

TARGET = dictionary_demo

all: $(TARGET)

$(TARGET): chaining_linprobing_bst.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(TARGET)
