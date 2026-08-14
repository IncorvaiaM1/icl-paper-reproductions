
#include "syr2k.h"

void kernel_syr2k(
    double alpha,
    double beta,
    double C[30][30],
    double A[30][20],
    double B[30][20]) {

    // Temporary matrix to store intermediate results
    double temp[30][30];

    // Initialize temp matrix to zero
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 30; j++) {
            temp[i][j] = 0.0;
        }
    }

    // Compute alpha * A * B^T + alpha * B * A^T
    // Since we're only storing lower triangular part, we compute for j <= i
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j <= i; j++) {
            // Compute dot product for A[i][:] * B[j][:] (which is B^T[:][j])
            double ab = 0.0;
            double ba = 0.0;
            for (int k = 0; k < 20; k++) {
                ab += A[i][k] * B[j][k];
                ba += B[i][k] * A[j][k];
            }
            temp[i][j] = alpha * ab + alpha * ba;
        }
    }

    // Update C matrix: C_out = temp + beta * C
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j <= i; j++) {
            C[i][j] = temp[i][j] + beta * C[i][j];
        }
    }

    // The upper triangular part remains unchanged (but since we're using lower triangular storage,
    // we don't need to compute it)
}
