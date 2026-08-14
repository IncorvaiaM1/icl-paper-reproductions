
#include "atax.h"

void kernel_atax(double A[38][42], double x[42], double y[42], double tmp[38]) {
    int i, j;

    // tmp = A * x
    for (i = 0; i < 38; i++) {
        double sum = 0.0;
        for (j = 0; j < 42; j++) {
            sum += A[i][j] * x[j];
        }
        tmp[i] = sum;
    }

    // y = A^T * tmp
    for (j = 0; j < 42; j++) {
        double sum = 0.0;
        for (i = 0; i < 38; i++) {
            sum += A[i][j] * tmp[i];
        }
        y[j] = sum;
    }
}
