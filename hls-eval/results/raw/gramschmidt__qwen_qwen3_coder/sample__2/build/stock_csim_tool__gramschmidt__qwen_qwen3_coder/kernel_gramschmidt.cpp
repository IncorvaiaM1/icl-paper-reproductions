
#include "gramschmidt.h"

void kernel_gramschmidt(double A[20][30], double R[30][30], double Q[20][30]) {
    int i, j, k;
    double norm;

    // Initialize Q and R matrices
    for (i = 0; i < 20; i++) {
        for (j = 0; j < 30; j++) {
            Q[i][j] = 0.0;
        }
    }

    for (i = 0; i < 30; i++) {
        for (j = 0; j < 30; j++) {
            R[i][j] = 0.0;
        }
    }

    // Modified Gram-Schmidt Algorithm
    for (k = 0; k < 30; k++) {
        // Compute norm of column k (normalization stage)
        norm = 0.0;
        for (i = 0; i < 20; i++) {
            norm += A[i][k] * A[i][k];
        }
        norm = sqrt(norm);
        
        // Store norm in diagonal element of R
        R[k][k] = norm;

        // Orthogonalization stage: compute column k of Q
        if (norm > 0) {
            for (i = 0; i < 20; i++) {
                Q[i][k] = A[i][k] / norm;
            }
        }

        // Update stage: update remaining columns
        for (j = k + 1; j < 30; j++) {
            // Compute R[k][j] = Q[:,k]^T * A[:,j]
            R[k][j] = 0.0;
            for (i = 0; i < 20; i++) {
                R[k][j] += Q[i][k] * A[i][j];
            }

            // Update A[:,j] = A[:,j] - R[k][j] * Q[:,k]
            for (i = 0; i < 20; i++) {
                A[i][j] = A[i][j] - R[k][j] * Q[i][k];
            }
        }
    }
}
