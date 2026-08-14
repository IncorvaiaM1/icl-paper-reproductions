
#include "gesummv.h"

void kernel_gesummv(double alpha, double beta, double A[30][30], double B[30][30], double tmp[30], double x[30], double y[30]) {
    // Compute matrix-vector product Ax and store in tmp
    for (int i = 0; i < 30; i++) {
        #pragma HLS PIPELINE II=1
        tmp[i] = 0.0;
        for (int j = 0; j < 30; j++) {
            #pragma HLS UNROLL
            tmp[i] += A[i][j] * x[j];
        }
    }

    // Compute final result y = alpha * tmp + beta * (Bx)
    for (int i = 0; i < 30; i++) {
        #pragma HLS PIPELINE II=1
        double sumB = 0.0;
        for (int j = 0; j < 30; j++) {
            #pragma HLS UNROLL
            sumB += B[i][j] * x[j];
        }
        y[i] = alpha * tmp[i] + beta * sumB;
    }
}
