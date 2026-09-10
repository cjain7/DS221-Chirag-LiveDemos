CXX = g++
CXXFLAGS = -O3 -std=c++17

TARGET = btree_demo

all: $(TARGET)

$(TARGET): bst_btree.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

clean:
	rm -f $(TARGET) *dat
