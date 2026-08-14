
#include "jacobi-1d.h"

void kernel_jacobi_1d(double A[30], double B[30]) {
    const int n = 30;
    const int tsteps = 20;

    // Perform tsteps iterations of the Jacobi computation
    for (int t = 0; t < tsteps; t++) {
        if (t % 2 == 0) {
            // Update B using values from A (current state)
            for (int i = 1; i < n-1; i++) {
                B[i] = (A[i-1] + A[i] + A[i+1]) / 3.0;
            }
            // Preserve boundary values for B
            B[0] = A[0];
            B[n-1] = A[n-1];
        } else {
            // Update A using values from B (current state)
            for (int i = 1; i < n-1; i++) {
                A[i] = (B[i-1] + B[i] + B[i+1]) / 3.0;
            }
            // Preserve boundary values for A
            A[0] = B[0];
            A[n-1] = B[n-1];
        }
    }

    // Ensure both arrays contain the final state (current time step)
    if (tsteps % 2 == 0) {
        // Final state is in A, so copy to B
        for (int i = 0; i < n; i++) {
            B[i] = A[i];
        }
    } else {
        // Final state is in B, so copy to A
        for (int i = 0; i < n; i++) {
            A[i] = B[i];
        }
    }
}
