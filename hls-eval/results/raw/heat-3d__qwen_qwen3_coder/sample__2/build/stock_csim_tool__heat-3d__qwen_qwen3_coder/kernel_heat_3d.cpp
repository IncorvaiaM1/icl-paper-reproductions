
#include "heat-3d.h"

void kernel_heat_3d(double A[10][10][10], double B[10][10][10]) {
    const double factor = 0.125;
    const int n = 10;
    const int tsteps = 20;

    // Time step loop
    for (int t = 0; t < tsteps; t++) {
        // Iterate over the 3D grid excluding boundary points
        for (int i = 1; i < n - 1; i++) {
            for (int j = 1; j < n - 1; j++) {
                for (int k = 1; k < n - 1; k++) {
                    // Compute the updated value based on the heat equation
                    B[i][j][k] = A[i][j][k]
                               + factor * (A[i+1][j][k] - 2.0 * A[i][j][k] + A[i-1][j][k])
                               + factor * (A[i][j+1][k] - 2.0 * A[i][j][k] + A[i][j-1][k])
                               + factor * (A[i][j][k+1] - 2.0 * A[i][j][k] + A[i][j][k-1]);
                }
            }
        }

        // Copy B back to A for the next time step
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                for (int k = 0; k < n; k++) {
                    A[i][j][k] = B[i][j][k];
                }
            }
        }
    }
}
