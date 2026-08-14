
#include "trmm.h"

void kernel_trmm(double alpha, double A[20][20], double B[20][30]) {
    // B = alpha * A * B, where A is lower triangular with unit diagonal.
    // Process rows in reverse order to perform the operation in place.
    for (int i = 19; i >= 0; i--) {
        for (int j = 0; j < 30; j++) {
            // Unit diagonal implies A[i][i] is treated as 1.0.
            double sum = B[i][j];
            for (int k = 0; k < i; k++) {
                sum += A[i][k] * B[k][j];
            }
            B[i][j] = alpha * sum;
        }
    }
}
