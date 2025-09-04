#include <iostream>
#include <queue>
#include <vector>
#include <iomanip>
#include <chrono>
#include <random>
#include <cmath>

// Heap sort using priority_queue
void heapSort(std::vector<int>& arr) {
  // Max heap
  std::priority_queue<int> pq;
  for (int x : arr) pq.push(x);
  for (int i = arr.size() - 1; i >= 0; i--) {
    arr[i] = pq.top();
    pq.pop();
  }
}

int main() {
  std::vector<int> sizes = {10000, 100000, 1000000, 10000000};

  for (int n : sizes) {
    // Generate random array
    std::vector<int> arr(n);
    for (int i = 0; i < n; i++) arr[i] = rand();

    // Copy for fair comparison
    std::vector<int> arr1 = arr;
    std::vector<int> arr2 = arr;

    // Measure heap sort
    auto start = std::chrono::high_resolution_clock::now();
    heapSort(arr1);
    auto end = std::chrono::high_resolution_clock::now();
    double t1 = std::chrono::duration<double>(end - start).count();

    // Measure std::sort (default implementation of C++)
    start = std::chrono::high_resolution_clock::now();
    std::sort(arr2.begin(), arr2.end());
    end = std::chrono::high_resolution_clock::now();
    double t2 = std::chrono::duration<double>(end - start).count();

    std::cout << "n=" << n
      << " | heapsort=" << std::fixed << std::setprecision(4) << t1 << "s"
      << " | std::sort=" << t2 << "s\n";
  }
}

