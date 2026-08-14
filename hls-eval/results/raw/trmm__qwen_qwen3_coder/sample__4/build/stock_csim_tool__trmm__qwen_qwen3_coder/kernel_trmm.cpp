
#include "trmm.h"

void kernel_trmm(double alpha, double A[20][20], double B[20][30]) {
    int i, j, k;
    double temp;

    for (j = 0; j < 30; j++) {
        for (i = 0; i < 20; i++) {
            temp = 0.0;
            for (k = 0; k <= i; k++) {
                temp += A[i][k] * B[k][j];
            }
            B[i][j] = alpha * temp;
        }
    }
}
