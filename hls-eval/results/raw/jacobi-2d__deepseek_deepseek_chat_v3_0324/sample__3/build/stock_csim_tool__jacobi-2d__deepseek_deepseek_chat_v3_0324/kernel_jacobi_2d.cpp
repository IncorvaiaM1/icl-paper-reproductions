
#include "jacobi-2d.h"

void kernel_jacobi_2d(double A[30][30], double B[30][30]) {
    const int tsteps = 20;
    const int n = 30;

    for (int t = 0; t < tsteps; t++) {
        // Determine source and destination arrays for ping-pong
        double (*src)[30] = (t % 2 == 0) ? A : B;
        double (*dst)[30] = (t % 2 == 0) ? B : A;

        // Perform stencil computation
        for (int i = 1; i < n-1; i++) {
            for (int j = 1; j < n-1; j++) {
                dst[i][j] = (src[i][j] + src[i-1][j] + src[i+1][j] + 
                            src[i][j-1] + src[i][j+1]) * 0.2;
            }
        }

        // Boundary conditions (copy edges)
        for (int i = 0; i < n; i++) {
            dst[0][i] = src[0][i];
            dst[n-1][i] = src[n-1][i];
            dst[i][0] = src[i][0];
            dst[i][n-1] = src[i][n-1];
        }
    }
}
