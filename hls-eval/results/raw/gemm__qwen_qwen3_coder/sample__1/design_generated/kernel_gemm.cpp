
#include "gemm.h"

void kernel_gemm(
    double alpha,
    double beta,
    double C[20][25],
    double A[20][30],
    double B[30][25]) {

    // Local buffers for matrices A and B to improve memory access patterns
    double local_A[20][30];
    double local_B[30][25];

    // Copy A into local buffer
    for (int i = 0; i < 20; i++) {
        for (int k = 0; k < 30; k++) {
            local_A[i][k] = A[i][k];
        }
    }

    // Copy B into local buffer
    for (int k = 0; k < 30; k++) {
        for (int j = 0; j < 25; j++) {
            local_B[k][j] = B[k][j];
        }
    }

    // Perform GEMM operation: C = alpha * A * B + beta * C
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 25; j++) {
            double sum = 0.0;
            for (int k = 0; k < 30; k++) {
                sum += local_A[i][k] * local_B[k][j];
            }
            C[i][j] = alpha * sum + beta * C[i][j];
        }
    }
}
