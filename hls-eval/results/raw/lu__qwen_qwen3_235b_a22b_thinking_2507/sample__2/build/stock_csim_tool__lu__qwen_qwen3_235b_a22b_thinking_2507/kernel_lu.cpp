
#include "lu.h"

void kernel_lu(double A[40][40]) {
    const int N = 40;
    for (int i = 0; i < N; i++) {
        for (int j = i; j < N; j++) {
            double sum = 0.0;
            for (int k = 0; k < i; k++) {
                sum += A[i][k] * A[k][j];
            }
            A[i][j] -= sum;
        }
        for (int j = i + 1; j < N; j++) {
            double sum = 0.0;
            for (int k = 0; k < i; k++) {
                sum += A[j][k] * A[k][i];
            }
            A[j][i] = (A[j][i] - sum) / A[i][i];
        }
    }
}
