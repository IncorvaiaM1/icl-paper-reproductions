
#include "atax.h"

void kernel_atax(double A[38][42], double x[42], double y[42], double tmp[38]) {
    // Initialize tmp vector to 0
    for (int i = 0; i < 38; i++) {
        tmp[i] = 0.0;
    }

    // Initialize y vector to 0
    for (int i = 0; i < 42; i++) {
        y[i] = 0.0;
    }

    // Compute tmp = A * x (matrix-vector multiplication)
    for (int i = 0; i < 38; i++) {
        for (int j = 0; j < 42; j++) {
            tmp[i] += A[i][j] * x[j];
        }
    }

    // Compute y = A^T * tmp (matrix-vector multiplication with transpose)
    for (int j = 0; j < 42; j++) {
        for (int i = 0; i < 38; i++) {
            y[j] += A[i][j] * tmp[i];
        }
    }
}
