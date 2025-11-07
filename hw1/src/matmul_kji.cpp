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
    int* A = new int[N * N];
    int* B = new int[N * N];
    int* C = new int[N * N]();

    for (int i = 0; i < N * N; ++i) {
        A[i] = rand() % 100;
        B[i] = rand() % 100;
    }

    auto start = chrono::high_resolution_clock::now();

    // Matrix multiplication
    for (int k = 0; k < N; ++k) {
        const int* Bk = &B[k * N];
        for (int j = 0; j < N; ++j) {
            int bkj = Bk[j];
            for (int i = 0; i < N; ++i) {
                C[i * N + j] += A[i * N + k] * bkj;
            }
        }
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