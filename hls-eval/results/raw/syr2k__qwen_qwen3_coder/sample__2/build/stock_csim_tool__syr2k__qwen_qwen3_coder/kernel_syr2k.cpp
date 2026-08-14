
#include "syr2k.h"

void kernel_syr2k(
    double alpha,
    double beta,
    double C[30][30],
    double A[30][20],
    double B[30][20]) {

    int i, j, k;
    double ABt_elem, BAt_elem;

    // Perform the symmetric rank-2k update: C := alpha*A*B^T + alpha*B*A^T + beta*C
    // Only update the lower triangular part of the matrix C (UPLO = 'L')
    for (i = 0; i < 30; i++) {
        for (j = 0; j <= i; j++) { // j <= i ensures we work on the lower triangle
            ABt_elem = 0.0;
            BAt_elem = 0.0;

            // Compute the (i,j)-th element of A*B^T and B*A^T
            for (k = 0; k < 20; k++) {
                ABt_elem += A[i][k] * B[j][k]; // A[i][k] * B[j][k] because B is not transposed in memory but logically transposed
                BAt_elem += B[i][k] * A[j][k]; // B[i][k] * A[j][k] because A is not transposed in memory but logically transposed
            }

            // Update C[i][j] in place
            C[i][j] = alpha * ABt_elem + alpha * BAt_elem + beta * C[i][j];
        }
    }
}
