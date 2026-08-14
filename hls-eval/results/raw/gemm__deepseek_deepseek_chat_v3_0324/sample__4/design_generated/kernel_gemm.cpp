
#include "gemm.h"

void kernel_gemm(
    double alpha,
    double beta,
    double C[20][25],
    double A[20][30],
    double B[30][25]) {

    // Temporary matrix to store intermediate results
    double temp[20][25];

    // Initialize temp matrix with zeros
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 25; j++) {
            temp[i][j] = 0.0;
        }
    }

    // Matrix multiplication: temp = A * B
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 25; j++) {
            for (int k = 0; k < 30; k++) {
                temp[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    // Compute final result: C = alpha * temp + beta * C
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 25; j++) {
            C[i][j] = alpha * temp[i][j] + beta * C[i][j];
        }
    }
}
