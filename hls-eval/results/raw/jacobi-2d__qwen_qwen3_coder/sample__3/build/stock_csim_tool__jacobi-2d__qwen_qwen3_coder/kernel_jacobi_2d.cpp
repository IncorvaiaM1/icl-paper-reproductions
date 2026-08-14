
#include "jacobi-2d.h"

void kernel_jacobi_2d(double A[30][30], double B[30][30]) {
    int t, i, j;

    for (t = 0; t < 20; t++) {
        if (t % 2 == 0) {
            for (i = 1; i < 29; i++) {
                for (j = 1; j < 29; j++) {
                    B[i][j] = 0.2 * (A[i][j] + A[i-1][j] + A[i+1][j] + A[i][j-1] + A[i][j+1]);
                }
            }
        } else {
            for (i = 1; i < 29; i++) {
                for (j = 1; j < 29; j++) {
                    A[i][j] = 0.2 * (B[i][j] + B[i-1][j] + B[i+1][j] + B[i][j-1] + B[i][j+1]);
                }
            }
        }
    }
    
    // If the number of iterations is odd, copy B to A for the final result
    if (t % 2 == 1) {
        for (i = 0; i < 30; i++) {
            for (j = 0; j < 30; j++) {
                A[i][j] = B[i][j];
            }
        }
    }
}
