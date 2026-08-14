
#include "trmm.h"

void kernel_trmm(double alpha, double A[20][20], double B[20][30]) {
    // HLS interfaces
    #pragma HLS INTERFACE m_axi port=A offset=slave bundle=gmem
    #pragma HLS INTERFACE m_axi port=B offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=alpha bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control

    // Local buffers for A and B
    double A_local[20][20];
    double B_local[20][30];

    // Partition local arrays for parallel access
    #pragma HLS ARRAY_PARTITION variable=A_local cyclic factor=4 dim=1
    #pragma HLS ARRAY_PARTITION variable=A_local cyclic factor=4 dim=2
    #pragma HLS ARRAY_PARTITION variable=B_local cyclic factor=4 dim=1
    #pragma HLS ARRAY_PARTITION variable=B_local cyclic factor=4 dim=2

    // Copy A to local memory
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
            #pragma HLS PIPELINE II=1
            A_local[i][j] = A[i][j];
        }
    }

    // Copy B to local memory
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 30; j++) {
            #pragma HLS PIPELINE II=1
            B_local[i][j] = B[i][j];
        }
    }

    // Triangular matrix multiplication: B = alpha * A * B
    // A is lower triangular with unit diagonal (DIAG='U')
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 30; j++) {
            #pragma HLS PIPELINE II=1
            double sum = 0.0;
            for (int k = 0; k <= i; k++) {
                #pragma HLS UNROLL
                // Unit diagonal: use 1.0 when k == i, otherwise use A_local[i][k]
                double a_val = (k == i) ? 1.0 : A_local[i][k];
                sum += a_val * B_local[k][j];
            }
            B_local[i][j] = alpha * sum;
        }
    }

    // Copy result back to B
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 30; j++) {
            #pragma HLS PIPELINE II=1
            B[i][j] = B_local[i][j];
        }
    }
}
