#include <iostream>
#include <chrono>
#include <cstdlib>

using namespace std;

#ifndef N
#define N 1024
#endif

#ifndef UNROLL
#define UNROLL 4  // supported: 4 or 8
#endif

int main() {
    // const int N = 1024; // Start small (e.g., 512) and scale up later
    cout << "Matrix size: " << N << "x" << N << endl;
    cout << "Unroll factor: " << UNROLL << endl;
    int* A = new int[N * N];
    int* B = new int[N * N];
    int* C = new int[N * N]();

    for (int i = 0; i < N * N; ++i) {
        A[i] = rand() % 100;
        B[i] = rand() % 100;
    }

    auto start = chrono::high_resolution_clock::now();

    // Matrix multiplication with manual unrolling over k
    for (int i = 0; i < N; ++i) {
        const int* Ai = &A[i * N];
        int* Ci = &C[i * N];
        for (int j = 0; j < N; ++j) {
            int sum = 0;
            int k = 0;

#if UNROLL == 8
            for (; k + 7 < N; k += 8) {
                sum += Ai[k + 0] * B[(k + 0) * N + j];
                sum += Ai[k + 1] * B[(k + 1) * N + j];
                sum += Ai[k + 2] * B[(k + 2) * N + j];
                sum += Ai[k + 3] * B[(k + 3) * N + j];
                sum += Ai[k + 4] * B[(k + 4) * N + j];
                sum += Ai[k + 5] * B[(k + 5) * N + j];
                sum += Ai[k + 6] * B[(k + 6) * N + j];
                sum += Ai[k + 7] * B[(k + 7) * N + j];
            }
            for (; k + 3 < N; k += 4) {
                sum += Ai[k + 0] * B[(k + 0) * N + j];
                sum += Ai[k + 1] * B[(k + 1) * N + j];
                sum += Ai[k + 2] * B[(k + 2) * N + j];
                sum += Ai[k + 3] * B[(k + 3) * N + j];
            }
#elif UNROLL == 4
            for (; k + 3 < N; k += 4) {
                sum += Ai[k + 0] * B[(k + 0) * N + j];
                sum += Ai[k + 1] * B[(k + 1) * N + j];
                sum += Ai[k + 2] * B[(k + 2) * N + j];
                sum += Ai[k + 3] * B[(k + 3) * N + j];
            }
#else
#warning "UNROLL must be 4 or 8; falling back to scalar loop"
#endif
            for (; k < N; ++k) {
                sum += Ai[k] * B[k * N + j];
            }
            Ci[j] = sum;
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