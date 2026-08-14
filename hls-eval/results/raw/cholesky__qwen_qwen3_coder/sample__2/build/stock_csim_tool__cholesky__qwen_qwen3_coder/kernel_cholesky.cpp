
#include "cholesky.h"

void kernel_cholesky(double A[40][40]) {
    int i, j, k;
    double sum;

    for (i = 0; i < 40; i++) {
        // Compute diagonal element A[i][i]
        sum = 0.0;
        for (k = 0; k < i; k++) {
            sum += A[i][k] * A[i][k];
        }
        A[i][i] = sqrt(A[i][i] - sum);

        // Compute elements below diagonal in column i
        for (j = i + 1; j < 40; j++) {
            sum = 0.0;
            for (k = 0; k < i; k++) {
                sum += A[j][k] * A[i][k];
            }
            A[j][i] = (A[j][i] - sum) / A[i][i];
        }
    }

    // Zero out upper triangular part
    for (i = 0; i < 40; i++) {
        for (j = i + 1; j < 40; j++) {
            A[i][j] = 0.0;
        }
    }
}
