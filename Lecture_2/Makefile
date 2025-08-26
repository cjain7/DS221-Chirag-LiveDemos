CXX = g++
CXXFLAGS = -O3 -std=c++11 -I/opt/homebrew/opt/openblas/include
LDFLAGS = -L/opt/homebrew/opt/openblas/lib -framework Accelerate

TARGET = matmul_demo

all: $(TARGET)

$(TARGET): matmul_demo.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGET)
