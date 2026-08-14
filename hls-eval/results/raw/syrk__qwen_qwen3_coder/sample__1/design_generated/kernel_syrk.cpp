
#include "syrk.h"

void kernel_syrk(double alpha, double beta, double C[30][30], double A[30][20]) {
    int i, j, k;
    
    // Temporary matrix to store the result of alpha * A * A^T
    double tmp_C[30][30];
    
    // Initialize tmp_C to zero
    for (i = 0; i < 30; i++) {
        for (j = 0; j < 30; j++) {
            tmp_C[i][j] = 0.0;
        }
    }

    // Compute alpha * A * A^T
    for (i = 0; i < 30; i++) {
        for (j = 0; j < 30; j++) {
            for (k = 0; k < 20; k++) {
                tmp_C[i][j] += alpha * A[i][k] * A[j][k];
            }
        }
    }

    // Update C = tmp_C + beta * C and store only the lower triangular part
    for (i = 0; i < 30; i++) {
        for (j = 0; j <= i; j++) {
            C[i][j] = tmp_C[i][j] + beta * C[i][j];
        }
    }
}
