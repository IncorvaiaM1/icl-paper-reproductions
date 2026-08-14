
#include "gemver.h"

void kernel_gemver(
    double alpha,
    double beta,
    double A[40][40],
    double u1[40],
    double v1[40],
    double u2[40],
    double v2[40],
    double w[40],
    double x[40],
    double y[40],
    double z[40])
{
    const int N = 40;

    // Stage 1: Matrix Update A = A + u1*v1^T + u2*v2^T
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i][j] = A[i][j] + u1[i] * v1[j] + u2[i] * v2[j];
        }
    }

    // Stage 2: Vector Update x = beta * A^T * y + z
    for (int i = 0; i < N; i++) {
        x[i] = z[i]; // Initialize with z
        for (int j = 0; j < N; j++) {
            x[i] += beta * A[j][i] * y[j]; // A^T is A[j][i] instead of A[i][j]
        }
    }

    // Stage 3: Final Vector Computation w = alpha * A * x
    for (int i = 0; i < N; i++) {
        w[i] = 0.0;
        for (int j = 0; j < N; j++) {
            w[i] += alpha * A[i][j] * x[j];
        }
    }
}
