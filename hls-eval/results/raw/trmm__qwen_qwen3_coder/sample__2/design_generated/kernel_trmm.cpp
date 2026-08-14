
#include "trmm.h"

void kernel_trmm(double alpha, double A[20][20], double B[20][30]) {
    int i, j, k;

    // Perform B = alpha * A * B
    // A is a lower triangular matrix (UPLO = 'L'), side = 'L', transa = 'N', diag = 'U'
    // Only the lower triangle of A is used, and the diagonal is assumed to be 1 (unit diagonal)

    for (i = 0; i < 20; i++) {
        for (j = 0; j < 30; j++) {
            double sum = 0.0;

            // Compute the dot product of row i of A with column j of B
            for (k = 0; k < i; k++) {
                sum += A[i][k] * B[k][j];
            }

            // Add the diagonal element (which is 1 due to DIAG = 'U')
            sum += B[i][j];

            // Store the result back in B, scaled by alpha
            B[i][j] = alpha * sum;
        }
    }
}
