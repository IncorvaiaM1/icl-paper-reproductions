
#include "symm.h"

void kernel_symm(
    double alpha,
    double beta,
    double C[20][30],
    double A[20][20],
    double B[20][30]) {

    #pragma HLS INTERFACE m_axi port=C bundle=gmem
    #pragma HLS INTERFACE m_axi port=A bundle=gmem
    #pragma HLS INTERFACE m_axi port=B bundle=gmem
    #pragma HLS INTERFACE s_axilite port=alpha bundle=control
    #pragma HLS INTERFACE s_axilite port=beta bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control

    double temp_C[20][30];

    // Initialize temp_C with zeros
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 30; j++) {
            temp_C[i][j] = 0.0;
        }
    }

    // Compute alpha*A*B (A is symmetric lower triangular)
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 30; j++) {
            for (int k = 0; k <= i; k++) {
                temp_C[i][j] += alpha * A[i][k] * B[k][j];
            }
            for (int k = i+1; k < 20; k++) {
                temp_C[i][j] += alpha * A[k][i] * B[k][j];
            }
        }
    }

    // Compute beta*B*C and add to temp_C
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 30; j++) {
            temp_C[i][j] += beta * B[i][j] * C[i][j];
        }
    }

    // Copy result back to C
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 30; j++) {
            C[i][j] = temp_C[i][j];
        }
    }
}
