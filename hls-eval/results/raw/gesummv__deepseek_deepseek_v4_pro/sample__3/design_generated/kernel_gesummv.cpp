
#include "gesummv.h"

void kernel_gesummv(
    double alpha,
    double beta,
    double A[30][30],
    double B[30][30],
    double tmp[30],
    double x[30],
    double y[30]) {
    const int N = 30;

    for (int i = 0; i < N; ++i) {
        double tmp_acc = 0.0;
        double y_acc = 0.0;

        for (int j = 0; j < N; ++j) {
            tmp_acc += A[i][j] * x[j];
            y_acc += B[i][j] * x[j];
        }

        tmp[i] = tmp_acc;
        y[i] = alpha * tmp_acc + beta * y_acc;
    }
}
