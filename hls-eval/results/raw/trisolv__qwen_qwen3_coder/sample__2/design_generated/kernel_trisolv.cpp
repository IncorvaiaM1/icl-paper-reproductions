
#include "trisolv.h"

void kernel_trisolv(double L[40][40], double x[40], double b[40]) {
    int i, j;
    double sum;

    for (i = 0; i < 40; i++) {
        sum = 0.0;
        for (j = 0; j < i; j++) {
            sum += L[i][j] * x[j];
        }
        x[i] = (b[i] - sum) / L[i][i];
    }
}
