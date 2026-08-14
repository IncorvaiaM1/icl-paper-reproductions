
#include "syrk.h"

void kernel_syrk(double alpha, double beta, double C[30][30], double A[30][20]) {
    int i, j, k;
    double temp;

    // Perform the SYRK operation: C = alpha * A * A^T + beta * C
    // Only update the lower triangular part of C (UPLO = 'L')
    for (i = 0; i < 30; i++) {
        for (j = 0; j <= i; j++) {
            temp = 0.0;
            for (k = 0; k < 20; k++) {
                temp += A[i][k] * A[j][k];
            }
            C[i][j] = alpha * temp + beta * C[i][j];
        }
    }
}
