
#include "doitgen.h"

void kernel_doitgen(double A[10][8][12], double C4[12][12], double sum[12]) {
    int r, q, p, s;

    for (r = 0; r < 10; r++) {
        for (q = 0; q < 8; q++) {
            // First, compute the sum over s for each p
            for (p = 0; p < 12; p++) {
                sum[p] = 0.0;
                for (s = 0; s < 12; s++) {
                    sum[p] += A[r][q][s] * C4[p][s];
                }
            }

            // Then, store the result back into A
            for (p = 0; p < 12; p++) {
                A[r][q][p] = sum[p];
            }
        }
    }
}
