
#include "syrk.h"

void kernel_syrk(double alpha, double beta, double C[30][30], double A[30][20]) {
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j <= i; j++) {
            double sum_val = 0.0;
            for (int k = 0; k < 20; k++) {
                sum_val += A[i][k] * A[j][k];
            }
            C[i][j] = alpha * sum_val + beta * C[i][j];
        }
    }
}
