
#include "seidel-2d.h"

void kernel_seidel_2d(double A[40][40]) {
    // Perform 20 time steps of Gauss-Seidel iteration
    for (int t = 0; t < 20; t++) {
        // Traverse interior points (boundaries remain fixed)
        for (int i = 1; i < 39; i++) {
            for (int j = 1; j < 39; j++) {
                // Calculate sum of 8 neighboring points (excluding center)
                double sum = A[i-1][j-1] + A[i-1][j] + A[i-1][j+1] +
                             A[i][j-1]             + A[i][j+1] +
                             A[i+1][j-1] + A[i+1][j] + A[i+1][j+1];
                
                // Update current point with average of neighbors (Gauss-Seidel style)
                A[i][j] = sum / 8.0;
            }
        }
    }
}
