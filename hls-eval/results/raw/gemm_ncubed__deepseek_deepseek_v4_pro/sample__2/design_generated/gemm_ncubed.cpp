
#include "gemm_ncubed.h"

void gemm(TYPE m1[N], TYPE m2[N], TYPE prod[N]) {
    int i, j, k;
    TYPE sum;

    for (i = 0; i < row_size; i++) {
        for (j = 0; j < col_size; j++) {
            sum = 0.0;
            for (k = 0; k < row_size; k++) {
                sum += m1[i * col_size + k] * m2[k * col_size + j];
            }
            prod[i * col_size + j] = sum;
        }
    }
}
