
#include "seidel-2d.h"

void kernel_seidel_2d(double A[40][40]) {
    const int N = 40;
    const int TSTEPS = 20;
    const double INV_9 = 1.0 / 9.0;

    for (int t = 0; t < TSTEPS; ++t) {
        for (int i = 1; i < N - 1; ++i) {
            for (int j = 1; j < N - 1; ++j) {
                double sum = A[i-1][j-1] + A[i-1][j] + A[i-1][j+1] +
                             A[i][j-1]   + A[i][j]   + A[i][j+1] +
                             A[i+1][j-1] + A[i+1][j] + A[i+1][j+1];
                A[i][j] = sum * INV_9;
            }
        }
    }
}
