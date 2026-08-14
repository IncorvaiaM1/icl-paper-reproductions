
#include "lu.h"

void kernel_lu(double A[40][40]) {
    double L[40][40];
    double U[40][40];

    // Initialize L and U matrices
    for (int i = 0; i < 40; i++) {
        for (int j = 0; j < 40; j++) {
            L[i][j] = 0.0;
            U[i][j] = 0.0;
        }
    }

    // LU decomposition without pivoting
    for (int i = 0; i < 40; i++) {
        // Compute U[i][j] for j >= i
        for (int j = i; j < 40; j++) {
            double sum = 0.0;
            for (int k = 0; k < i; k++) {
                sum += L[i][k] * U[k][j];
            }
            U[i][j] = A[i][j] - sum;
        }

        // Compute L[j][i] for j > i
        for (int j = i + 1; j < 40; j++) {
            double sum = 0.0;
            for (int k = 0; k < i; k++) {
                sum += L[j][k] * U[k][i];
            }
            L[j][i] = (A[j][i] - sum) / U[i][i];
        }

        // Set diagonal element of L to 1
        L[i][i] = 1.0;
    }

    // Copy L and U back to A
    for (int i = 0; i < 40; i++) {
        for (int j = 0; j < 40; j++) {
            if (i > j) {
                A[i][j] = L[i][j];
            } else {
                A[i][j] = U[i][j];
            }
        }
    }
}
