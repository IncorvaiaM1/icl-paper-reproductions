
#include "syr2k.h"

void kernel_syr2k(double alpha, double beta, double C[30][30], double A[30][20], double B[30][20]) {
    int i, j, k;

    for (i = 0; i < 30; i++) {
        for (j = 0; j <= i; j++) {
            C[i][j] = beta * C[i][j];

            for (k = 0; k < 20; k++) {
                C[i][j] += alpha * A[i][k] * B[j][k] + alpha * B[i][k] * A[j][k];
            }
        }
    }
}
