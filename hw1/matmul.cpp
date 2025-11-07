#include <iostream>
#include <chrono>
#include <cstdlib>

using namespace std;

#ifndef N
#define N 1024
#endif

int main() {
    // const int N = 1024; // Start small (e.g., 512) and scale up later
    cout << "Matrix size: " << N << "x" << N << endl;
    double* A = new double[N * N];
    double* B = new double[N * N];
    double* C = new double[N * N]();

    for (int i = 0; i < N * N; ++i) {
        A[i] = rand() % 100;
        B[i] = rand() % 100;
    }

    auto start = chrono::high_resolution_clock::now();

    // Matrix multiplication
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j) {
            double sum = 0;
            for (int k = 0; k < N; ++k)
                sum += A[i * N + k] * B[k * N + j];
            C[i * N + j] = sum;
        }

    auto end = chrono::high_resolution_clock::now();

    double time_taken = chrono::duration<double>(end - start).count();
    cout << "Execution time: " << time_taken << " seconds" << endl;

    double checksum = 0;
    for (int i = 0; i < N * N; ++i)
        checksum += C[i];

    cout << "Checksum: " << checksum << endl;

    delete[] A;
    delete[] B;
    delete[] C;
}