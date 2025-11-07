#include <iostream>
#include <chrono>
#include <cstdlib>
#include <algorithm>

using namespace std;

#ifndef N
#define N 1024
#endif

#ifndef TILE
#define TILE 32
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
    // Tiled (blocked) matrix multiplication: ii-kk-jj outer blocks, inner ikj
    for (int ii = 0; ii < N; ii += TILE) {
        int iimax = std::min(ii + TILE, N);
        for (int kk = 0; kk < N; kk += TILE) {
            int kkmax = std::min(kk + TILE, N);
            for (int jj = 0; jj < N; jj += TILE) {
                int jjmax = std::min(jj + TILE, N);
                for (int i = ii; i < iimax; ++i) {
                    int* Ci = &C[i * N];
                    for (int k = kk; k < kkmax; ++k) {
                        int aik = A[i * N + k];
                        const int* Bk = &B[k * N];
                        for (int j = jj; j < jjmax; ++j) {
                            Ci[j] += aik * Bk[j];
                        }
                    }
                }
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