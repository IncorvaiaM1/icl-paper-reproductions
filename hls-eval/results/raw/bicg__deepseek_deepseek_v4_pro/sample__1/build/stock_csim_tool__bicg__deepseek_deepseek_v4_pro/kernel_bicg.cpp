
#include "bicg.h"

void kernel_bicg(
    double A[42][38],
    double s[38],
    double q[42],
    double p[38],
    double r[42])
{
    // Compute q = A * p
    // q is of length 42, p is of length 38
    for (int i = 0; i < 42; i++) {
        double sum = 0.0;
        for (int j = 0; j < 38; j++) {
            sum += A[i][j] * p[j];
        }
        q[i] = sum;
    }

    // Compute s = A^T * r
    // s is of length 38, r is of length 42
    for (int j = 0; j < 38; j++) {
        double sum = 0.0;
        for (int i = 0; i < 42; i++) {
            sum += A[i][j] * r[i];
        }
        s[j] = sum;
    }
}
