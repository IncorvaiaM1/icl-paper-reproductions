
#include "mvt.h"

void kernel_mvt(
    double x1[40],
    double x2[40],
    double y_1[40],
    double y_2[40],
    double A[40][40]) {

    int i, j;

    // Compute x1 = x1 + A * y1
    for (i = 0; i < 40; i++) {
        double sum = 0.0;
        for (j = 0; j < 40; j++) {
            sum += A[i][j] * y_1[j];
        }
        x1[i] += sum;
    }

    // Compute x2 = x2 + A^T * y2
    for (i = 0; i < 40; i++) {
        double sum = 0.0;
        for (j = 0; j < 40; j++) {
            sum += A[j][i] * y_2[j];
        }
        x2[i] += sum;
    }
}
