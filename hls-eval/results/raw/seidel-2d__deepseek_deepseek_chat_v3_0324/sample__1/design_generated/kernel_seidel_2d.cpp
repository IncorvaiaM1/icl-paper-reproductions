
#include "seidel-2d.h"

void kernel_seidel_2d(double A[40][40]) {
    const int TSTEPS = 20;
    const int N = 40;
    
    double A_copy[40][40];
    
    // Main computation loop
    for (int t = 0; t < TSTEPS; t++) {
        // Create a copy of A for this time step
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++) {
                A_copy[i][j] = A[i][j];
            }
        }
        
        // Update each element using the 9-point stencil
        for (int i = 1; i < N-1; i++) {
            for (int j = 1; j < N-1; j++) {
                A[i][j] = (A_copy[i-1][j-1] + A_copy[i-1][j] + A_copy[i-1][j+1] +
                           A_copy[i][j-1]   + A_copy[i][j]   + A_copy[i][j+1] +
                           A_copy[i+1][j-1] + A_copy[i+1][j] + A_copy[i+1][j+1]) / 9.0;
            }
        }
    }
}
