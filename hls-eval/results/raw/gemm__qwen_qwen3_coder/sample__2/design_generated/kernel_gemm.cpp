
#include "gemm.h"

void kernel_gemm(
    double alpha,
    double beta,
    double C[20][25],
    double A[20][30],
    double B[30][25]) {

    // Temporary matrix to store the result of A * B
    double tmp_C[20][25];

    // Perform matrix multiplication: tmp_C = A * B
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 25; j++) {
            tmp_C[i][j] = 0.0;
            for (int k = 0; k < 30; k++) {
                tmp_C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    // Perform scaling and accumulation: C = alpha * tmp_C + beta * C
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 25; j++) {
            C[i][j] = alpha * tmp_C[i][j] + beta * C[i][j];
        }
    }
}
