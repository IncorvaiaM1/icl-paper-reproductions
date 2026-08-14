
#include "lu.h"

void kernel_lu(double A[40][40]) {
    int i, j, k;
    double L[40][40];
    double U[40][40];

    // Initialize L and U matrices
    for (i = 0; i < 40; i++) {
        for (j = 0; j < 40; j++) {
            L[i][j] = 0.0;
            U[i][j] = 0.0;
        }
    }

    // LU decomposition without pivoting
    for (i = 0; i < 40; i++) {
        // Compute U(i,j) for j >= i
        for (j = i; j < 40; j++) {
            double sum = 0.0;
            for (k = 0; k < i; k++) {
                sum += L[i][k] * U[k][j];
            }
            U[i][j] = A[i][j] - sum;
        }

        // Compute L(j,i) for j > i
        L[i][i] = 1.0; // Diagonal elements of L are 1
        for (j = i + 1; j < 40; j++) {
            double sum = 0.0;
            for (k = 0; k < i; k++) {
                sum += L[j][k] * U[k][i];
            }
            L[j][i] = (A[j][i] - sum) / U[i][i];
        }
    }

    // Copy the result back to A
    for (i = 0; i < 40; i++) {
        for (j = 0; j < 40; j++) {
            if (j < i) {
                A[i][j] = L[i][j];
            } else {
                A[i][j] = U[i][j];
            }
        }
    }
}
