
#include "syrk.h"

void kernel_syrk(double alpha, double beta, double C[30][30], double A[30][20]) {
    int i, j, k;

    // Iterate over the lower triangular part of the matrix C
    for (i = 0; i < 30; i++) {
        for (j = 0; j <= i; j++) {
            double sum = 0.0;

            // Compute the dot product of row i of A and row j of A
            for (k = 0; k < 20; k++) {
                sum += A[i][k] * A[j][k];
            }

            // Update C[i][j] = alpha * (A * A^T)[i][j] + beta * C[i][j]
            C[i][j] = alpha * sum + beta * C[i][j];
        }
    }
}
