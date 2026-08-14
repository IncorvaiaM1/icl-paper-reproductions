
#include "gesummv.h"

void kernel_gesummv(
    double alpha,
    double beta,
    double A[30][30],
    double B[30][30],
    double tmp[30],
    double x[30],
    double y[30])
{
    // Compute tmp = A*x and y = B*x
    for (int i = 0; i < 30; i++) {
        tmp[i] = 0.0;
        y[i] = 0.0;
        for (int j = 0; j < 30; j++) {
            tmp[i] += A[i][j] * x[j];
            y[i] += B[i][j] * x[j];
        }
    }

    // Compute y = alpha*tmp + beta*y
    for (int i = 0; i < 30; i++) {
        y[i] = alpha * tmp[i] + beta * y[i];
    }
}
