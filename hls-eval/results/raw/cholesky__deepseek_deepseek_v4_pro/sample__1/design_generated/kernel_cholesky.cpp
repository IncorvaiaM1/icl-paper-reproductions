
#include "cholesky.h"

void kernel_cholesky(double A[40][40]) {
    const int N = 40;

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < i; ++j) {
            double sum = A[i][j];
            for (int k = 0; k < j; ++k) {
                sum -= A[i][k] * A[j][k];
            }
            A[i][j] = sum / A[j][j];
        }

        double diag_sum = A[i][i];
        for (int k = 0; k < i; ++k) {
            diag_sum -= A[i][k] * A[i][k];
        }
        A[i][i] = std::sqrt(diag_sum);
    }

    for (int i = 0; i < N; ++i) {
        for (int j = i + 1; j < N; ++j) {
            A[i][j] = 0.0;
        }
    }
}
