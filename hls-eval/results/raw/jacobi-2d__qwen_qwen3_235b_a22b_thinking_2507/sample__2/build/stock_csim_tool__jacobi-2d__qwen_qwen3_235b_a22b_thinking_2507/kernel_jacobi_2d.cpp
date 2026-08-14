
#include "jacobi-2d.h"

void kernel_jacobi_2d(double A[30][30], double B[30][30]) {
    for (int t = 0; t < 10; t++) {
        for (int i = 1; i < 29; i++) {
            for (int j = 1; j < 29; j++) {
#pragma HLS PIPELINE
                B[i][j] = 0.2 * (A[i][j] + A[i-1][j] + A[i+1][j] + A[i][j-1] + A[i][j+1]);
            }
        }
        
        for (int i = 1; i < 29; i++) {
            for (int j = 1; j < 29; j++) {
#pragma HLS PIPELINE
                A[i][j] = 0.2 * (B[i][j] + B[i-1][j] + B[i+1][j] + B[i][j-1] + B[i][j+1]);
            }
        }
    }
}
