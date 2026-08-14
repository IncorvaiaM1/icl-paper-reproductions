
#include "doitgen.h"

void kernel_doitgen(double A[10][8][12], double C4[12][12], double sum[12]) {
    const int R = 10;
    const int Q = 8;
    const int S = 12;
    const int P = 12;

    // Temporary buffer for storing intermediate results
    double A_temp[R][Q][P];

    // Perform the matrix multiplication-like operation
    for (int r = 0; r < R; r++) {
        for (int q = 0; q < Q; q++) {
            for (int p = 0; p < P; p++) {
                sum[p] = 0.0;
                for (int s = 0; s < S; s++) {
                    sum[p] += A[r][q][s] * C4[p][s];
                }
                A_temp[r][q][p] = sum[p];
            }
        }
    }

    // Copy results back to A (in-place operation)
    for (int r = 0; r < R; r++) {
        for (int q = 0; q < Q; q++) {
            for (int p = 0; p < P; p++) {
                A[r][q][p] = A_temp[r][q][p];
            }
        }
    }
}
