
#include "symm.h"

void kernel_symm(double alpha, double beta, double C[20][30], double A[20][20], double B[20][30]) {
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 30; j++) {
            double sum = 0.0;

            for (int k = 0; k <= i; k++) {
                sum += A[i][k] * B[k][j];
            }
            for (int k = i + 1; k < 20; k++) {
                sum += A[k][i] * B[k][j];
            }

            C[i][j] = alpha * sum + beta * C[i][j];
        }
    }
}
