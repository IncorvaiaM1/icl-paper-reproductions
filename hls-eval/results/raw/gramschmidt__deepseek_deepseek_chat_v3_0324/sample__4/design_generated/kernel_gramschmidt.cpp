
#include "gramschmidt.h"
#include <cmath>

void kernel_gramschmidt(double A[20][30], double R[30][30], double Q[20][30]) {
    const int M = 20;
    const int N = 30;

    // Modified Gram-Schmidt QR decomposition
    for (int k = 0; k < N; k++) {
        // Compute R[k][k] as the norm of column k of A
        double norm = 0.0;
        for (int i = 0; i < M; i++) {
            norm += A[i][k] * A[i][k];
        }
        R[k][k] = sqrt(norm);

        // Compute Q column k (normalization)
        for (int i = 0; i < M; i++) {
            Q[i][k] = A[i][k] / R[k][k];
        }

        // Update remaining columns of A and compute R[k][j] for j > k
        for (int j = k + 1; j < N; j++) {
            // Compute R[k][j] as dot product of Q column k and A column j
            R[k][j] = 0.0;
            for (int i = 0; i < M; i++) {
                R[k][j] += Q[i][k] * A[i][j];
            }

            // Update A column j
            for (int i = 0; i < M; i++) {
                A[i][j] = A[i][j] - Q[i][k] * R[k][j];
            }
        }
    }
}
