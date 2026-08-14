
#include "gramschmidt.h"

void kernel_gramschmidt(double A[20][30], double R[30][30], double Q[20][30]) {
    const int m = 20;
    const int n = 30;
    const double tol = 1e-10;

    for (int k = 0; k < n; k++) {
        double norm = 0.0;
        for (int i = 0; i < m; i++) {
            norm += A[i][k] * A[i][k];
        }
        norm = std::sqrt(norm);

        if (norm < tol) {
            R[k][k] = 0.0;
            for (int i = 0; i < m; i++) {
                Q[i][k] = 0.0;
            }
        } else {
            R[k][k] = norm;
            for (int i = 0; i < m; i++) {
                Q[i][k] = A[i][k] / norm;
            }
        }

        for (int j = k + 1; j < n; j++) {
            double dot = 0.0;
            for (int i = 0; i < m; i++) {
                dot += Q[i][k] * A[i][j];
            }
            R[k][j] = dot;

            for (int i = 0; i < m; i++) {
                A[i][j] = A[i][j] - dot * Q[i][k];
            }
        }
    }
}
