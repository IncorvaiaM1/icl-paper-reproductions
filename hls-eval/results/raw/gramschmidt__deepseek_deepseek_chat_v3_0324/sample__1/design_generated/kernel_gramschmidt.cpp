
#include "gramschmidt.h"
#include <cmath>

void kernel_gramschmidt(double A[20][30], double R[30][30], double Q[20][30]) {
    const int M = 20;
    const int N = 30;

    // Normalization stage
    for (int k = 0; k < N; k++) {
        // Compute norm of column k of A
        double norm = 0.0;
        for (int i = 0; i < M; i++) {
            norm += A[i][k] * A[i][k];
        }
        norm = std::sqrt(norm);
        R[k][k] = norm;

        // Compute Q column k
        for (int i = 0; i < M; i++) {
            Q[i][k] = A[i][k] / norm;
        }

        // Orthogonalization and update stages
        for (int j = k + 1; j < N; j++) {
            // Compute R[k][j] as dot product of Q column k and A column j
            double dot_product = 0.0;
            for (int i = 0; i < M; i++) {
                dot_product += Q[i][k] * A[i][j];
            }
            R[k][j] = dot_product;

            // Update A column j
            for (int i = 0; i < M; i++) {
                A[i][j] = A[i][j] - Q[i][k] * R[k][j];
            }
        }
    }
}
