
#include "atax.h"

void kernel_atax(double A[38][42], double x[42], double y[42], double tmp[38]) {
    int i, j;

    for (i = 0; i &lt; 38; i++) {
        double sum = 0.0;
        for (j = 0; j &lt; 42; j++) {
            sum += A[i][j] * x[j];
        }
        tmp[i] = sum;
    }

    for (j = 0; j &lt; 42; j++) {
        double sum = 0.0;
        for (i = 0; i &lt; 38; i++) {
            sum += A[i][j] * tmp[i];
        }
        y[j] = sum;
    }
}
