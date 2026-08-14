
#include "gramschmidt.h"

void kernel_gramschmidt(double A[20][30], double R[30][30], double Q[20][30]) {
    int k, j, i;
    double norm, temp;

    // Modified Gram-Schmidt algorithm
    for (k = 0; k < 30; k++) {
        // Compute the norm of the k-th column of A
        norm = 0.0;
        for (i = 0; i < 20; i++) {
            norm += A[i][k] * A[i][k];
        }
        norm = sqrt(norm);

        // Store the norm in the diagonal element of R
        R[k][k] = norm;

        // Normalize the k-th column of A to produce the k-th column of Q
        if (norm != 0.0) {
            for (i = 0; i < 20; i++) {
                Q[i][k] = A[i][k] / norm;
            }
        } else {
            for (i = 0; i < 20; i++) {
                Q[i][k] = 0.0;
            }
        }

        // Update the remaining columns of A
        for (j = k + 1; j < 30; j++) {
            // Compute the dot product of the k-th column of Q and the j-th column of A
            temp = 0.0;
            for (i = 0; i < 20; i++) {
                temp += Q[i][k] * A[i][j];
            }

            // Store the dot product in the (k,j) element of R
            R[k][j] = temp;

            // Update the j-th column of A
            for (i = 0; i < 20; i++) {
                A[i][j] = A[i][j] - Q[i][k] * temp;
            }
        }
    }
}
