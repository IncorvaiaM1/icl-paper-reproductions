
#include "syrk.h"

void kernel_syrk(double alpha, double beta, double C[30][30], double A[30][20]) {
    // N = 30, M = 20
    // UPLO = 'L' : only lower triangular part of C is updated
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j <= i; j++) {
            double sum = 0.0;
            for (int k = 0; k < 20; k++) {
                sum += A[i][k] * A[j][k];
            }
            C[i][j] = beta * C[i][j] + alpha * sum;
        }
    }
}
