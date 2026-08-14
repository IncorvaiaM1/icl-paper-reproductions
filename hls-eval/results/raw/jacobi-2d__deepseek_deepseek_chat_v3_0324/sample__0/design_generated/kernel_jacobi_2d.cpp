
#include "jacobi-2d.h"

void kernel_jacobi_2d(double A[30][30], double B[30][30]) {
    const int TSTEPS = 20;
    const int N = 30;
    
    for (int t = 0; t < TSTEPS; t++) {
        // Alternate between A and B as source and destination
        if (t % 2 == 0) {
            // Compute from A to B
            for (int i = 1; i < N - 1; i++) {
                for (int j = 1; j < N - 1; j++) {
                    B[i][j] = (A[i][j] + A[i-1][j] + A[i+1][j] + A[i][j-1] + A[i][j+1]) / 5.0;
                }
            }
        } else {
            // Compute from B to A
            for (int i = 1; i < N - 1; i++) {
                for (int j = 1; j < N - 1; j++) {
                    A[i][j] = (B[i][j] + B[i-1][j] + B[i+1][j] + B[i][j-1] + B[i][j+1]) / 5.0;
                }
            }
        }
    }

    // Final copy if odd number of time steps
    if (TSTEPS % 2 == 1) {
        for (int i = 1; i < N - 1; i++) {
            for (int j = 1; j < N - 1; j++) {
                A[i][j] = B[i][j];
            }
        }
    }
}
