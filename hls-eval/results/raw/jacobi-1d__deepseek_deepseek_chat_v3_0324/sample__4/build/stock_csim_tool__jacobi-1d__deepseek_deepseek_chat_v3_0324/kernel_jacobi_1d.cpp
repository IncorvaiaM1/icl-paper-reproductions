
#include "jacobi-1d.h"

void kernel_jacobi_1d(double A[30], double B[30]) {
    #pragma HLS INTERFACE m_axi port=A bundle=gmem0
    #pragma HLS INTERFACE m_axi port=B bundle=gmem1
    #pragma HLS INTERFACE s_axilite port=A bundle=control
    #pragma HLS INTERFACE s_axilite port=B bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control

    const int n = 30;
    const int tsteps = 20;
    double temp[30];

    // Copy input data to temporary array
    copy_loop:
    for (int i = 0; i < n; i++) {
        #pragma HLS PIPELINE II=1
        temp[i] = A[i];
    }

    // Perform Jacobi iterations
    time_loop:
    for (int t = 0; t < tsteps; t++) {
        #pragma HLS LOOP_TRIPCOUNT min=20 max=20

        // Compute new values
        stencil_loop:
        for (int i = 1; i < n - 1; i++) {
            #pragma HLS PIPELINE II=1
            B[i] = (temp[i-1] + temp[i] + temp[i+1]) * 0.333333;
        }

        // Handle boundary conditions
        B[0] = temp[0];
        B[n-1] = temp[n-1];

        // Update temp array for next iteration
        update_loop:
        for (int i = 0; i < n; i++) {
            #pragma HLS PIPELINE II=1
            temp[i] = B[i];
        }
    }

    // Copy results back to output array
    output_loop:
    for (int i = 0; i < n; i++) {
        #pragma HLS PIPELINE II=1
        A[i] = B[i];
    }
}
