
#include "syr2k.h"

void kernel_syr2k(
    double alpha,
    double beta,
    double C[30][30],
    double A[30][20],
    double B[30][20]) {

    int i, j, k;
    double tmp1, tmp2;

    // Perform the symmetric rank 2k update
    // C := alpha*A*B**T + alpha*B*A**T + beta*C
    // Since UPLO = 'L', only update the lower triangle of C

    for (i = 0; i < 30; i++) {
        for (j = 0; j <= i; j++) { // Only update lower triangle (j <= i)
            tmp1 = 0.0;
            tmp2 = 0.0;

            // Compute dot products for AB^T and BA^T
            for (k = 0; k < 20; k++) {
                tmp1 += A[i][k] * B[j][k]; // A[i][k] * B[j][k] for AB^T
                tmp2 += B[i][k] * A[j][k]; // B[i][k] * A[j][k] for BA^T
            }

            // Update C[i][j] in place
            C[i][j] = alpha * tmp1 + alpha * tmp2 + beta * C[i][j];
        }
    }
}
