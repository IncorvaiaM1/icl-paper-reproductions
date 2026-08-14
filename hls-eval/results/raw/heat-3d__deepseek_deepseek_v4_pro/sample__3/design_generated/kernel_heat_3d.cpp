
#include "heat-3d.h"

void kernel_heat_3d(double A[10][10][10], double B[10][10][10]) {
    const int N = 10;
    const int TSTEPS = 20;

    for (int t = 0; t < TSTEPS; ++t) {
        for (int i = 1; i < N - 1; ++i) {
            for (int j = 1; j < N - 1; ++j) {
                for (int k = 1; k < N - 1; ++k) {
                    double center = A[i][j][k];
                    B[i][j][k] = center
                        + 0.125 * (A[i + 1][j][k] - 2.0 * center + A[i - 1][j][k])
                        + 0.125 * (A[i][j + 1][k] - 2.0 * center + A[i][j - 1][k])
                        + 0.125 * (A[i][j][k + 1] - 2.0 * center + A[i][j][k - 1]);
                }
            }
        }

        for (int i = 1; i < N - 1; ++i) {
            for (int j = 1; j < N - 1; ++j) {
                for (int k = 1; k < N - 1; ++k) {
                    A[i][j][k] = B[i][j][k];
                }
            }
        }
    }
}
