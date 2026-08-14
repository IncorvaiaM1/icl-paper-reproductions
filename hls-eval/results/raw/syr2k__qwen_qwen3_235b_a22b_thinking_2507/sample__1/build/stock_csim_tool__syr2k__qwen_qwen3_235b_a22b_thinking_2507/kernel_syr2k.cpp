
#include "syr2k.h"

void kernel_syr2k(double alpha, double beta, double C[30][30], double A[30][20], double B[30][20]) {
    // Scale the entire C matrix by beta
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 30; j++) {
            C[i][j] = beta * C[i][j];
        }
    }

    // Perform the symmetric rank-2k update: C += alpha * (A * B^T + B * A^T)
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 30; j++) {
            for (int k = 0; k < 20; k++) {
                C[i][j] += alpha * (A[i][k] * B[j][k] + B[i][k] * A[j][k]);
            }
        }
    }
}
