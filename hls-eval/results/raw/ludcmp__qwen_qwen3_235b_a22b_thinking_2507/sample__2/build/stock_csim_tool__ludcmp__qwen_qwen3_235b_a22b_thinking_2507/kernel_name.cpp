
#include "ludcmp.h"

void kernel_ludcmp(double A[40][40], double b[40], double x[40], double y[40]) {
    // Perform LU decomposition in-place on matrix A
    for (int k = 0; k < 40; k++) {
        // Compute upper triangular part (U) for row k
        for (int j = k; j < 40; j++) {
            double sum = 0.0;
            for (int p = 0; p < k; p++) {
                sum += A[k][p] * A[p][j];
            }
            A[k][j] -= sum;
        }
        
        // Compute lower triangular part (L) for column k (excluding diagonal)
        for (int i = k + 1; i < 40; i++) {
            double sum = 0.0;
            for (int p = 0; p < k; p++) {
                sum += A[i][p] * A[p][k];
            }
            A[i][k] = (A[i][k] - sum) / A[k][k];
        }
    }
    
    // Forward substitution: Solve Ly = b for y
    for (int i = 0; i < 40; i++) {
        y[i] = b[i];
        for (int j = 0; j < i; j++) {
            y[i] -= A[i][j] * y[j];
        }
        // Note: L[i][i] = 1, so no division needed
    }
    
    // Backward substitution: Solve Ux = y for x
    for (int i = 39; i >= 0; i--) {
        x[i] = y[i];
        for (int j = i + 1; j < 40; j++) {
            x[i] -= A[i][j] * x[j];
        }
        x[i] = x[i] / A[i][i];
    }
}
