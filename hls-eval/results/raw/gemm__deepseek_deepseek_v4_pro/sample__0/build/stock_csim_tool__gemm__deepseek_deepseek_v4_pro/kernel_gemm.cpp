
#include "gemm.h"

void kernel_gemm(
    double alpha,
    double beta,
    double C[20][25],
    double A[20][30],
    double B[30][25]) {
    
    // Temporary buffer to store the result of A * B
    double temp[20][25];
    
    // Initialize temp to zero
    #pragma HLS ARRAY_PARTITION variable=temp complete dim=0
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 25; j++) {
            #pragma HLS UNROLL
            temp[i][j] = 0.0;
        }
    }
    
    // Compute temp = A * B
    for (int i = 0; i < 20; i++) {
        for (int k = 0; k < 30; k++) {
            #pragma HLS PIPELINE II=1
            for (int j = 0; j < 25; j++) {
                #pragma HLS UNROLL
                temp[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    
    // Compute C = alpha * temp + beta * C
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 25; j++) {
            #pragma HLS UNROLL
            C[i][j] = alpha * temp[i][j] + beta * C[i][j];
        }
    }
}
