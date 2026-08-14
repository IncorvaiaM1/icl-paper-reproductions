
#include "syr2k.h"

void kernel_syr2k(double alpha, double beta, double C[30][30], double A[30][20], double B[30][20]) {
    // Update the lower triangular part (including diagonal) of matrix C
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j <= i; j++) {
            double sum = 0.0;
            for (int k = 0; k < 20; k++) {
                // Compute alpha * (A[i][k] * B[j][k] + B[i][k] * A[j][k])
                sum += A[i][k] * B[j][k] + B[i][k] * A[j][k];
            }
            // Apply the full operation: C[i][j] = alpha * sum + beta * C[i][j]
            C[i][j] = alpha * sum + beta * C[i][j];
        }
    }

    // Mirror the updated lower triangular part to the upper triangular part
    // to maintain symmetry (since C must remain symmetric)
    for (int i = 0; i < 30; i++) {
        for (int j = i + 1; j < 30; j++) {
            C[i][j] = C[j][i];
        }
    }
}
