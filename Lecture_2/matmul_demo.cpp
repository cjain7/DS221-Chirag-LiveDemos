#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <cmath>

// CBLAS header
#include <cblas.h>

// Naïve O(N^3) matrix multiplication
void matmul_naive(const std::vector<double> &A,
                  const std::vector<double> &B,
                  std::vector<double> &C,
                  int N) {
    for (int i = 0; i < N; i++) {  //ith row of A
        for (int j = 0; j < N; j++) {  //jth column of B
            double sum = 0.0;
            for (int k = 0; k < N; k++) {  // dot product
                sum += A[i*N + k] * B[k*N + j];  
            }
            C[i*N + j] = sum; //row major storage of elements 
        }
    }
}

int main(int argc, char** argv) {
    int N = 2000; // dimension of our NxN dense matrix

    std::cout << "\033[1m" << "Matrix multiplication demo with N=" << N << "\033[0m" << std::endl;

    std::vector<double> A(N*N), B(N*N), C(N*N);

    // Fill the matrix with random numbers
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    for (int i = 0; i < N*N; i++) {
        A[i] = dist(rng);
        B[i] = dist(rng);
    }

    // --- Naïve multiplication ---
    {
        auto start = std::chrono::high_resolution_clock::now();
        matmul_naive(A, B, C, N);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "\033[1m" << "Naïve multiplication time: " << elapsed.count() << " secods" << "\033[0m" << std::endl;
    }

    // Verify correctness roughly
    std::cout << "C[0,0] = " << C[0] << "  (first element in output matrix C)\n";

    // --- Using BLAS library ---
    {
        auto start = std::chrono::high_resolution_clock::now();
        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
                    N, N, N,
                    1.0, A.data(), N,
                    B.data(), N,
                    0.0, C.data(), N);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "\033[1m" << "BLAS multiplication time: " << elapsed.count() << " seconds" << "\033[0m" << std::endl;
    }

    // Verify correctness roughly
    // This is not the ideal way to check correctness
    std::cout << "C[0,0] = " << C[0] << "  (first element in output matrix C)\n";

    return 0;
}

