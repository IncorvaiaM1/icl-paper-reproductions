
#include "symm.h"

void kernel_symm(double alpha, double beta, double C[20][30], double A[20][20], double B[20][30]) {
    const int M = 20;
    const int N = 30;

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            double sum = 0.0;
            for (int k = 0; k < M; k++) {
                double a_ik;
                if (i >= k) {
                    a_ik = A[i][k];
                } else {
                    a_ik = A[k][i];
                }
                sum += a_ik * B[k][j];
            }
            C[i][j] = beta * C[i][j] + alpha * sum;
        }
    }
}
