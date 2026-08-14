
#include "symm.h"

void kernel_symm(
    double alpha,
    double beta,
    double C[20][30],
    double A[20][20],
    double B[20][30]) {

    // Temporary storage for intermediate results
    double temp[20][30] = {0};

    // Compute alpha * A * B (A is symmetric lower triangular)
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 30; j++) {
            double sum = 0.0;
            // Only use lower triangular part of A
            for (int k = 0; k <= i; k++) {
                sum += A[i][k] * B[k][j];
            }
            // For symmetric upper part (k > i), use A[k][i] = A[i][k]
            for (int k = i + 1; k < 20; k++) {
                sum += A[k][i] * B[k][j];
            }
            temp[i][j] = alpha * sum;
        }
    }

    // Compute beta * B * C and add to temp result
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 30; j++) {
            temp[i][j] += beta * B[i][j] * C[i][j];
        }
    }

    // Write final result back to C
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 30; j++) {
            C[i][j] = temp[i][j];
        }
    }
}
