
#include "trmm.h"

void kernel_trmm(double alpha, double A[20][20], double B[20][30]) {
    double temp_B[20][30];

    // Initialize temp_B with original B values
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 30; j++) {
            temp_B[i][j] = B[i][j];
        }
    }

    // Perform TRMM operation: B = alpha * A * B
    // Since A is lower triangular with unit diagonal (DIAG='U'), we skip the diagonal multiplication
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 30; j++) {
            double sum = 0.0;
            // Multiply row i of A with column j of B
            for (int k = 0; k <= i; k++) {
                if (k < i) {  // Off-diagonal elements
                    sum += A[i][k] * temp_B[k][j];
                } else {  // Diagonal elements (A[k][k] = 1.0 due to unit diagonal)
                    sum += temp_B[k][j];
                }
            }
            B[i][j] = alpha * sum;
        }
    }
}
