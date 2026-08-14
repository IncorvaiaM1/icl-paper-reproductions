
#include "gemver.h"

void kernel_gemver(
    double alpha,
    double beta,
    double A[40][40],
    double u1[40],
    double v1[40],
    double u2[40],
    double v2[40],
    double w[40],
    double x[40],
    double y[40],
    double z[40]) {

    // Stage 1: Matrix update (A = A + u1 * v1^T + u2 * v2^T)
    for (int i = 0; i < 40; i++) {
        #pragma HLS LOOP_TRIPCOUNT min=40 max=40
        for (int j = 0; j < 40; j++) {
            #pragma HLS LOOP_TRIPCOUNT min=40 max=40
            #pragma HLS PIPELINE
            A[i][j] += u1[i] * v1[j] + u2[i] * v2[j];
        }
    }

    // Stage 2: Vector update (x = beta * A^T * y + z)
    for (int i = 0; i < 40; i++) {
        #pragma HLS LOOP_TRIPCOUNT min=40 max=40
        double sum = 0.0;
        for (int k = 0; k < 40; k++) {
            #pragma HLS LOOP_TRIPCOUNT min=40 max=40
            sum += A[k][i] * y[k];
        }
        x[i] = beta * sum + z[i];
    }

    // Stage 3: Final vector computation (w = alpha * A * x)
    for (int i = 0; i < 40; i++) {
        #pragma HLS LOOP_TRIPCOUNT min=40 max=40
        double sum = 0.0;
        for (int k = 0; k < 40; k++) {
            #pragma HLS LOOP_TRIPCOUNT min=40 max=40
            sum += A[i][k] * x[k];
        }
        w[i] = alpha * sum;
    }
}
