
#include "lu.h"

void kernel_lu(double A[40][40]) {
    for (int k = 0; k < 40; ++k) {
        for (int j = k; j < 40; ++j) {
            double sum = 0.0;
            for (int p = 0; p < k; ++p) {
                sum += A[k][p] * A[p][j];
            }
            A[k][j] -= sum;
        }
        for (int i = k + 1; i < 40; ++i) {
            double sum = 0.0;
            for (int p = 0; p < k; ++p) {
                sum += A[i][p] * A[p][k];
            }
            A[i][k] = (A[i][k] - sum) / A[k][k];
        }
    }
}
