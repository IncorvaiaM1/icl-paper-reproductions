
#include "syr2k.h"

void kernel_syr2k(
    double alpha,
    double beta,
    double C[30][30],
    double A[30][20],
    double B[30][20])
{
    // Perform symmetric rank 2k update: C = alpha*(A*B^T + B*A^T) + beta*C
    // Only lower triangular part of C is computed and stored

    const int N = 30;
    const int M = 20;

    // Temporary storage for intermediate results
    double temp[30][30] = {0};

    // Compute A*B^T and accumulate in temp
    for (int i = 0; i < N; i++) {
        for (int j = 0; j <= i; j++) {  // Only compute lower triangular part
            for (int k = 0; k < M; k++) {
                temp[i][j] += A[i][k] * B[j][k];
            }
        }
    }

    // Compute B*A^T and accumulate in temp
    for (int i = 0; i < N; i++) {
        for (int j = 0; j <= i; j++) {  // Only compute lower triangular part
            for (int k = 0; k < M; k++) {
                temp[i][j] += B[i][k] * A[j][k];
            }
            // Scale by alpha and add beta*C
            C[i][j] = alpha * temp[i][j] + beta * C[i][j];
        }
    }

    // The upper triangular part remains unchanged as per BLAS syr2k behavior
    // with UPLO='L'
}
