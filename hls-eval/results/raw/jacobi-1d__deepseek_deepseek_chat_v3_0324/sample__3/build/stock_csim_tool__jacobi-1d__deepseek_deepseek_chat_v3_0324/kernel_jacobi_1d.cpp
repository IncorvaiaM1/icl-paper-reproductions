
#include "jacobi-1d.h"

void kernel_jacobi_1d(double A[30], double B[30]) {
    const int n = 30;
    const int tsteps = 20;
    double temp[30];

    for (int t = 0; t < tsteps; t++) {
        // Compute new values
        for (int i = 1; i < n - 1; i++) {
            temp[i] = (A[i-1] + A[i] + A[i+1]) / 3.0;
        }

        // Handle boundary conditions (unchanged)
        temp[0] = A[0];
        temp[n-1] = A[n-1];

        // Copy temp to B for this iteration
        for (int i = 0; i < n; i++) {
            B[i] = temp[i];
        }

        // Swap A and B for next iteration
        if (t < tsteps - 1) {
            for (int i = 0; i < n; i++) {
                A[i] = B[i];
            }
        }
    }
}
