
#include "seidel-2d.h"

void kernel_seidel_2d(double A[40][40]) {
    const int TSTEPS = 20;
    const int N = 40;
    
    double A_copy[40][40];
    
    // Copy the input array
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            A_copy[i][j] = A[i][j];
        }
    }
    
    // Perform Gauss-Seidel iterations
    for (int t = 0; t < TSTEPS; ++t) {
        for (int i = 1; i < N-1; ++i) {
            for (int j = 1; j < N-1; ++j) {
                // Compute the average of the 9-point stencil
                A[i][j] = (A_copy[i-1][j-1] + A_copy[i-1][j] + A_copy[i-1][j+1] +
                           A_copy[i][j-1]   + A_copy[i][j]   + A_copy[i][j+1] +
                           A_copy[i+1][j-1] + A_copy[i+1][j] + A_copy[i+1][j+1]) / 9.0;
            }
        }
        
        // Update the copy for the next iteration
        for (int i = 1; i < N-1; ++i) {
            for (int j = 1; j < N-1; ++j) {
                A_copy[i][j] = A[i][j];
            }
        }
    }
}
