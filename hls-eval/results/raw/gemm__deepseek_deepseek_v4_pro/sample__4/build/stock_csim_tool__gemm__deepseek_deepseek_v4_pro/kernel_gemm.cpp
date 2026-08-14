
#include "gemm.h"

void kernel_gemm(
    double alpha,
    double beta,
    double C[20][25],
    double A[20][30],
    double B[30][25]) {
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 25; j++) {
            double sum = 0.0;
            for (int k = 0; k < 30; k++) {
                sum += A[i][k] * B[k][j];
            }
            C[i][j] = alpha * sum + beta * C[i][j];
        }
    }
}
