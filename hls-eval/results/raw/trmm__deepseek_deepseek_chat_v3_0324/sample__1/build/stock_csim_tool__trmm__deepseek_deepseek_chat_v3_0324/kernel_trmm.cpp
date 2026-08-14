
#include "trmm.h"

void kernel_trmm(double alpha, double A[20][20], double B[20][30]) {
    double temp_B[20][30];

    // Initialize temp_B with B values
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 30; j++) {
            temp_B[i][j] = B[i][j];
        }
    }

    // Perform TRMM operation: B = alpha * A * B
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 30; j++) {
            double sum = 0.0;
            // Since A is lower triangular, we only multiply up to i
            for (int k = 0; k <= i; k++) {
                sum += A[i][k] * temp_B[k][j];
            }
            B[i][j] = alpha * sum;
        }
    }
}
