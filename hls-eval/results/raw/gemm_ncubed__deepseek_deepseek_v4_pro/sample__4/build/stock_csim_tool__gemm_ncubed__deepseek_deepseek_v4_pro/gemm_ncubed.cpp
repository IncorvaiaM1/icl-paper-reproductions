
#include "gemm_ncubed.h"

void gemm(TYPE m1[N], TYPE m2[N], TYPE prod[N]) {
    int i, j, k;
    TYPE sum;

    for (i = 0; i < row_size; i++) {
        for (j = 0; j < col_size; j++) {
            sum = 0.0;

            for (k = 0; k < col_size; k++) {
                sum += m1[i * row_size + k] * m2[k * row_size + j];
            }

            prod[i * row_size + j] = sum;
        }
    }
}
