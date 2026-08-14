
#include "lu.h"

void kernel_lu(double A[40][40]) {
    #pragma HLS INTERFACE m_axi port=A bundle=gmem
    #pragma HLS INTERFACE s_axilite port=A bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control

    double L[40][40] = {0};
    double U[40][40] = {0};

    for (int i = 0; i < 40; i++) {
        #pragma HLS PIPELINE II=1
        for (int j = 0; j < 40; j++) {
            #pragma HLS PIPELINE II=1
            if (i <= j) {
                // Compute U[i][j]
                double sum = 0.0;
                for (int k = 0; k < i; k++) {
                    #pragma HLS PIPELINE II=1
                    sum += L[i][k] * U[k][j];
                }
                U[i][j] = A[i][j] - sum;
            }
            
            if (i > j) {
                // Compute L[i][j]
                double sum = 0.0;
                for (int k = 0; k < j; k++) {
                    #pragma HLS PIPELINE II=1
                    sum += L[i][k] * U[k][j];
                }
                L[i][j] = (A[i][j] - sum) / U[j][j];
            }
        }
    }

    // Copy results back to A (L in lower part, U in upper part)
    for (int i = 0; i < 40; i++) {
        #pragma HLS PIPELINE II=1
        for (int j = 0; j < 40; j++) {
            #pragma HLS PIPELINE II=1
            if (i > j) {
                A[i][j] = L[i][j];
            } else {
                A[i][j] = U[i][j];
            }
        }
    }
}
