
#include "doitgen.h"

void kernel_doitgen(double A[10][8][12], double C4[12][12], double sum[12]) {
    int r, q, p, s;

    for (r = 0; r < 10; r++) {
        for (q = 0; q < 8; q++) {
            for (s = 0; s < 12; s++) {
                sum[s] = 0.0;
                for (p = 0; p < 12; p++) {
                    sum[s] += A[r][q][p] * C4[s][p];
                }
            }
            for (s = 0; s < 12; s++) {
                A[r][q][s] = sum[s];
            }
        }
    }
}
