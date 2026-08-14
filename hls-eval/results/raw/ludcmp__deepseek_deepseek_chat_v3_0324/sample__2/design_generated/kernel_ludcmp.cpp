
#include "ludcmp.h"
#include <cmath>

void kernel_ludcmp(double A[40][40], double b[40], double x[40], double y[40]) {
    const int N = 40;

    // LU Decomposition (in-place)
    for(int k = 0; k < N; k++) {
        // Compute U elements (row k)
        for(int j = k; j < N; j++) {
            double sum = 0.0;
            for(int p = 0; p < k; p++) {
                sum += A[k][p] * A[p][j];
            }
            A[k][j] = A[k][j] - sum;
        }

        // Compute L elements (column k)
        for(int i = k+1; i < N; i++) {
            double sum = 0.0;
            for(int p = 0; p < k; p++) {
                sum += A[i][p] * A[p][k];
            }
            A[i][k] = (A[i][k] - sum) / A[k][k];
        }
    }

    // Forward substitution (Ly = b)
    for(int i = 0; i < N; i++) {
        double sum = 0.0;
        for(int j = 0; j < i; j++) {
            sum += A[i][j] * y[j];
        }
        y[i] = (b[i] - sum) / A[i][i];
    }

    // Backward substitution (Ux = y)
    for(int i = N-1; i >= 0; i--) {
        double sum = 0.0;
        for(int j = i+1; j < N; j++) {
            sum += A[i][j] * x[j];
        }
        x[i] = (y[i] - sum) / A[i][i];
    }
}
