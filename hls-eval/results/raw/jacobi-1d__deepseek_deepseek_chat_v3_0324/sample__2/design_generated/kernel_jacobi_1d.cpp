
#include "jacobi-1d.h"

void kernel_jacobi_1d(double A[30], double B[30]) {
    const int n = 30;
    const int tsteps = 20;
    double temp_A[30];
    
    // Initialize temp array
    for (int i = 0; i < n; i++) {
        temp_A[i] = A[i];
    }

    // Time-stepping loop
    for (int t = 0; t < tsteps; t++) {
        // Compute stencil for each point (excluding boundaries)
        for (int i = 1; i < n-1; i++) {
            B[i] = (temp_A[i-1] + temp_A[i] + temp_A[i+1]) / 3.0;
        }

        // Handle boundary conditions (copy from input)
        B[0] = temp_A[0];
        B[n-1] = temp_A[n-1];

        // Update temp array for next time step
        for (int i = 0; i < n; i++) {
            temp_A[i] = B[i];
        }
    }

    // Copy final results back to A
    for (int i = 0; i < n; i++) {
        A[i] = temp_A[i];
    }
}
