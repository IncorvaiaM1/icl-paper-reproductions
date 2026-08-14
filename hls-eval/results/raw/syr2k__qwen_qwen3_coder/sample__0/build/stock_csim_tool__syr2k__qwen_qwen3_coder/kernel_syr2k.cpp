
#include "syr2k.h"

void kernel_syr2k(
    double alpha,
    double beta,
    double C[30][30],
    double A[30][20],
    double B[30][20]) {

    int i, j, k;
    double ABt[30][30];
    double BAt[30][30];

    // Step 1: Compute AB^T
    for (i = 0; i < 30; i++) {
        for (j = 0; j < 30; j++) {
            ABt[i][j] = 0.0;
            for (k = 0; k < 20; k++) {
                ABt[i][j] += A[i][k] * B[j][k];
            }
        }
    }

    // Step 2: Compute BA^T
    for (i = 0; i < 30; i++) {
        for (j = 0; j < 30; j++) {
            BAt[i][j] = 0.0;
            for (k = 0; k < 20; k++) {
                BAt[i][j] += B[i][k] * A[j][k];
            }
        }
    }

    // Step 3: Update C = alpha * (AB^T + BA^T) + beta * C
    // Only update the lower triangular part (UPLO = 'L')
    for (i = 0; i < 30; i++) {
        for (j = 0; j <= i; j++) {
            C[i][j] = alpha * (ABt[i][j] + BAt[i][j]) + beta * C[i][j];
        }
    }
}
