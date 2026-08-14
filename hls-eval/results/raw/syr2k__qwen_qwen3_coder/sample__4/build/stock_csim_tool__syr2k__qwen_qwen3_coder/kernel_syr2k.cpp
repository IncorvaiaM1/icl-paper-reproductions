
#include "syr2k.h"

void kernel_syr2k(
    double alpha,
    double beta,
    double C[30][30],
    double A[30][20],
    double B[30][20]) {

    int i, j, k;
    double temp1, temp2;

    // Compute C = alpha * A * B^T + alpha * B * A^T + beta * C
    // Only update the lower triangular part of C (UPLO = 'L')
    for (i = 0; i < 30; i++) {
        for (j = 0; j <= i; j++) {
            temp1 = 0.0;
            temp2 = 0.0;
            for (k = 0; k < 20; k++) {
                temp1 += A[i][k] * B[j][k];
                temp2 += B[i][k] * A[j][k];
            }
            C[i][j] = alpha * temp1 + alpha * temp2 + beta * C[i][j];
        }
    }
}
