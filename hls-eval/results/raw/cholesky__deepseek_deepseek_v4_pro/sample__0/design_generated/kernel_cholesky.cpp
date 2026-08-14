
#include "cholesky.h"

void kernel_cholesky(double A[40][40]) {
    const int N = 40;

    for (int i = 0; i < N; i++) {
        // Compute off-diagonal elements of row i
        for (int j = 0; j < i; j++) {
            double sum = A[i][j];
            for (int k = 0; k < j; k++) {
                sum -= A[i][k] * A[j][k];
            }
            A[i][j] = sum / A[j][j];
        }

        // Compute diagonal element of row i
        double sum_diag = A[i][i];
        for (int k = 0; k < i; k++) {
            sum_diag -= A[i][k] * A[i][k];
        }
        A[i][i] = std::sqrt(sum_diag);
    }
}
