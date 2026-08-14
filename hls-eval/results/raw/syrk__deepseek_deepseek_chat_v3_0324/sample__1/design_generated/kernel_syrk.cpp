
#include "syrk.h"

void kernel_syrk(double alpha, double beta, double C[30][30], double A[30][20]) {
    // Temporary matrix to store intermediate results
    double temp[30][30];

    // Initialize temp matrix to zero
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 30; j++) {
            temp[i][j] = 0.0;
        }
    }

    // Compute alpha * A * A^T (only lower triangular part)
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j <= i; j++) {
            for (int k = 0; k < 20; k++) {
                temp[i][j] += alpha * A[i][k] * A[j][k];
            }
        }
    }

    // Update C with beta * C + temp (only lower triangular part)
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j <= i; j++) {
            C[i][j] = beta * C[i][j] + temp[i][j];
        }
    }
}
