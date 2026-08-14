
#include "symm.h"

void kernel_symm(double alpha, double beta, double C[20][30], double A[20][20], double B[20][30]) {
    double temp[20][30];

    // Initialize temp to 0
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 30; j++) {
            temp[i][j] = 0.0;
        }
    }

    // Compute alpha * A * B and store in temp
    // Since A is symmetric lower triangular, we only use the lower triangular part
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 30; j++) {
            for (int k = 0; k <= i; k++) {
                temp[i][j] += alpha * A[i][k] * B[k][j];
            }
            // Add symmetric part (upper triangular elements)
            for (int k = i + 1; k < 20; k++) {
                temp[i][j] += alpha * A[k][i] * B[k][j];
            }
        }
    }

    // Compute beta * C and add to temp
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 30; j++) {
            temp[i][j] += beta * C[i][j];
        }
    }

    // Copy result back to C
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 30; j++) {
            C[i][j] = temp[i][j];
        }
    }
}
