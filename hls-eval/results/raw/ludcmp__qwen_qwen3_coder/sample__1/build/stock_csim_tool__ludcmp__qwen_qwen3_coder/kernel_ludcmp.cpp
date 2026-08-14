
#include "ludcmp.h"

void kernel_ludcmp(double A[40][40], double b[40], double x[40], double y[40]) {
    int i, j, k;
    double sum;

    // LU Decomposition (in-place on A)
    for (k = 0; k < 40; k++) {
        // Compute L matrix (below diagonal)
        for (i = k + 1; i < 40; i++) {
            A[i][k] = A[i][k] / A[k][k];
        }

        // Compute U matrix (above diagonal and diagonal)
        for (i = k + 1; i < 40; i++) {
            for (j = k + 1; j < 40; j++) {
                A[i][j] = A[i][j] - A[i][k] * A[k][j];
            }
        }
    }

    // Forward substitution: Ly = b
    for (i = 0; i < 40; i++) {
        sum = b[i];
        for (j = 0; j < i; j++) {
            sum = sum - A[i][j] * y[j];
        }
        y[i] = sum;
    }

    // Backward substitution: Ux = y
    for (i = 39; i >= 0; i--) {
        sum = y[i];
        for (j = i + 1; j < 40; j++) {
            sum = sum - A[i][j] * x[j];
        }
        x[i] = sum / A[i][i];
    }
}
