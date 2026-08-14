
#include "doitgen.h"

void kernel_doitgen(double A[10][8][12], double C4[12][12], double sum[12]) {
    for (int r = 0; r < 10; r++) {
        for (int q = 0; q < 8; q++) {
            for (int p = 0; p < 12; p++) {
                sum[p] = 0.0;
                for (int s = 0; s < 12; s++) {
                    sum[p] += A[r][q][s] * C4[p][s];
                }
            }
            for (int p = 0; p < 12; p++) {
                A[r][q][p] = sum[p];
            }
        }
    }
}
