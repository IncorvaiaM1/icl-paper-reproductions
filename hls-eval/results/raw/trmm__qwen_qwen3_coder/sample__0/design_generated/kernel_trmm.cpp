
#include "trmm.h"

void kernel_trmm(double alpha, double A[20][20], double B[20][30]) {
    int i, j, k;

    // Triple nested loop for TRMM computation
    // A is 20x20 lower triangular (UPLO = 'L'), unit diagonal (DIAG = 'U')
    // Multiplication from the left (SIDE = 'L'), not transposed (TRANSA = 'N')
    LOOP_I:
    for (i = 0; i < 20; i++) {
        LOOP_J:
        for (j = 0; j < 30; j++) {
            double sum = 0.0;

            LOOP_K:
            for (k = 0; k <= i; k++) {
                if (k == i) {
                    // Unit diagonal: A[i][i] is treated as 1.0
                    sum += B[k][j];
                } else {
                    sum += A[i][k] * B[k][j];
                }
            }

            B[i][j] = alpha * sum;
        }
    }
}
