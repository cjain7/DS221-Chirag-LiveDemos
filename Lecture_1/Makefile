# Makefile for compiling the data structure demo

CXX = g++
CXXFLAGS = -std=c++17 -O3 -w
TARGET = course_demo
SRC = Vector_vs_list.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
