
#include "cholesky.h"

void kernel_cholesky(double A[40][40]) {
    for (int i = 0; i < 40; i++) {
        for (int j = 0; j <= i; j++) {
            if (i == j) {
                double sum = 0.0;
                for (int k = 0; k < i; k++) {
                    sum += A[i][k] * A[i][k];
                }
                A[i][i] = sqrt(A[i][i] - sum);
            } else {
                double sum = 0.0;
                for (int k = 0; k < j; k++) {
                    sum += A[i][k] * A[j][k];
                }
                A[i][j] = (A[i][j] - sum) / A[j][j];
            }
        }
    }
}
