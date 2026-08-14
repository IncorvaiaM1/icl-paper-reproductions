
#include "gesummv.h"

void kernel_gesummv(
    double alpha,
    double beta,
    double A[30][30],
    double B[30][30],
    double tmp[30],
    double x[30],
    double y[30]) {

    int i, j;

    loop_i:
    for (i = 0; i < 30; i++) {
        double tmp_sum = 0.0;
        double y_sum = 0.0;

        loop_j:
        for (j = 0; j < 30; j++) {
            tmp_sum += A[i][j] * x[j];
            y_sum += B[i][j] * x[j];
        }

        tmp[i] = tmp_sum;
        y[i] = alpha * tmp[i] + beta * y_sum;
    }
}
