
#include "ludcmp.h"

void kernel_ludcmp(double A[40][40], double b[40], double x[40], double y[40]) {
    const int N = 40;

    // LU Decomposition (Doolittle's algorithm)
    for (int i = 0; i < N; i++) {
        // Upper triangular (U)
        for (int j = i; j < N; j++) {
            double sum = 0;
            for (int k = 0; k < i; k++) {
                sum += A[i][k] * A[k][j];
            }
            A[i][j] = A[i][j] - sum;
        }

        // Lower triangular (L)
        for (int j = i + 1; j < N; j++) {
            double sum = 0;
            for (int k = 0; k < i; k++) {
                sum += A[j][k] * A[k][i];
            }
            A[j][i] = (A[j][i] - sum) / A[i][i];
        }
    }

    // Forward substitution (Ly = b)
    for (int i = 0; i < N; i++) {
        double sum = 0;
        for (int j = 0; j < i; j++) {
            sum += A[i][j] * y[j];
        }
        y[i] = (b[i] - sum) / A[i][i];
    }

    // Backward substitution (Ux = y)
    for (int i = N - 1; i >= 0; i--) {
        double sum = 0;
        for (int j = i + 1; j < N; j++) {
            sum += A[i][j] * x[j];
        }
        x[i] = (y[i] - sum) / A[i][i];
    }
}
