
#include "syr2k.h"

void kernel_syr2k(
    double alpha,
    double beta,
    double C[30][30],
    double A[30][20],
    double B[30][20]) {
    // Configuration: TRANS = 'N' (no transpose), UPLO = 'L' (lower triangular)
    // C = alpha * A * B^T + alpha * B * A^T + beta * C
    // Only the lower triangular part of C (i >= j) is computed and stored.
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j <= i; j++) {
            double sum = 0.0;
            for (int k = 0; k < 20; k++) {
                sum += A[i][k] * B[j][k] + B[i][k] * A[j][k];
            }
            C[i][j] = beta * C[i][j] + alpha * sum;
        }
    }
}
