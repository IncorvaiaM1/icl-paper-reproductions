
#include "syrk.h"

void kernel_syrk(double alpha, double beta, double C[30][30], double A[30][20]) {
    // Temporary matrix to store intermediate results
    double temp[30][30] = {0};

    // Compute alpha * A * A^T
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 30; j++) {
            if (j <= i) { // Lower triangular part only
                double sum = 0.0;
                for (int k = 0; k < 20; k++) {
                    sum += A[i][k] * A[j][k];
                }
                temp[i][j] = alpha * sum;
            }
        }
    }

    // Update C with beta * C + alpha * A * A^T
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 30; j++) {
            if (j <= i) { // Lower triangular part only
                C[i][j] = beta * C[i][j] + temp[i][j];
            }
        }
    }
}
