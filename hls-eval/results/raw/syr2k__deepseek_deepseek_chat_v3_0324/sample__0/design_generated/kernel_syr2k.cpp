
#include "syr2k.h"

void kernel_syr2k(
    double alpha,
    double beta,
    double C[30][30],
    double A[30][20],
    double B[30][20]) {

    const int N = 30;
    const int M = 20;

    // Temporary storage for the result
    double C_temp[30][30];

    // Initialize C_temp with beta*C (lower triangular part only)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j <= i; j++) {
            C_temp[i][j] = beta * C[i][j];
        }
    }

    // Perform the symmetric rank-2k update: C = alpha*(A*B^T + B*A^T) + beta*C
    for (int i = 0; i < N; i++) {
        for (int j = 0; j <= i; j++) {
            double sum1 = 0.0;
            double sum2 = 0.0;
            
            // Compute A*B^T and B*A^T
            for (int k = 0; k < M; k++) {
                sum1 += A[i][k] * B[j][k];
                sum2 += B[i][k] * A[j][k];
            }
            
            // Update only the lower triangular part
            C_temp[i][j] += alpha * (sum1 + sum2);
        }
    }

    // Copy the result back to C (only lower triangular part)
    for (int i = 0; i < N; i++) {
        for (int j = 0; j <= i; j++) {
            C[i][j] = C_temp[i][j];
        }
    }
}
