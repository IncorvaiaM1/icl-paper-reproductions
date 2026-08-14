
#include "symm.h"

void kernel_symm(
    double alpha,
    double beta,
    double C[20][30],
    double A[20][20],
    double B[20][30]) {

    // Matrix dimensions
    const int M = 20;
    const int N = 30;

    // Symmetric matrix multiplication: C = alpha * A * B + beta * C
    // A is lower triangular symmetric (UPLO='L'), so A[i][k] = A[k][i] for i < k.
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            double temp = 0.0;

            // Compute dot product of row i of A and column j of B
            for (int k = 0; k < M; k++) {
                // Access A[i][k] using lower-triangular storage
                double a_ik = (i >= k) ? A[i][k] : A[k][i];
                temp += a_ik * B[k][j];
            }

            // Update C in place: C = alpha * A*B + beta * C
            C[i][j] = alpha * temp + beta * C[i][j];
        }
    }
}
