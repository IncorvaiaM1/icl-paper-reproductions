
#include "atax.h"

void kernel_atax(double A[38][42], double x[42], double y[42], double tmp[38]) {
    // Compute intermediate vector: tmp = A * x
    for (int i = 0; i < 38; i++) {
        double sum = 0.0;
        for (int j = 0; j < 42; j++) {
            sum += A[i][j] * x[j];
        }
        tmp[i] = sum;
    }

    // Compute final result: y = A^T * tmp
    for (int j = 0; j < 42; j++) {
        double sum = 0.0;
        for (int i = 0; i < 38; i++) {
            sum += A[i][j] * tmp[i];
        }
        y[j] = sum;
    }
}
