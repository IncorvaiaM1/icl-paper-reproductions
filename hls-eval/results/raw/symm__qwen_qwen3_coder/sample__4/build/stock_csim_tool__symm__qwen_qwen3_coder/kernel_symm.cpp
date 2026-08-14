
#include "symm.h"

void kernel_symm(
    double alpha,
    double beta,
    double C[20][30],
    double A[20][20],
    double B[20][30]) {

    int i, j, k;
    double temp;

    // Perform the symmetric matrix multiplication: C = alpha * A * B + beta * C
    for (i = 0; i < 20; i++) {
        for (j = 0; j < 30; j++) {
            temp = 0.0;
            for (k = 0; k < 20; k++) {
                // Since A is symmetric and stored as lower triangular,
                // use A[k][i] if k > i, otherwise A[i][k]
                if (k > i)
                    temp += A[k][i] * B[k][j];
                else
                    temp += A[i][k] * B[k][j];
            }
            C[i][j] = alpha * temp + beta * C[i][j];
        }
    }
}
