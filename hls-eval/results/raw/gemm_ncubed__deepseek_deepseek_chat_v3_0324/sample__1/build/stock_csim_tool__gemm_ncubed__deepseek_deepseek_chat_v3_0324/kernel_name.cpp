
#include "gemm_ncubed.h"

void gemm(TYPE m1[N], TYPE m2[N], TYPE prod[N]) {
    // Initialize output matrix to zero
    for (int i = 0; i < N; i++) {
        prod[i] = 0;
    }

    // Matrix multiplication
    for (int i = 0; i < row_size; i++) {
        for (int j = 0; j < col_size; j++) {
            TYPE sum = 0;
            for (int k = 0; k < col_size; k++) {
                sum += m1[i * col_size + k] * m2[k * col_size + j];
            }
            prod[i * col_size + j] = sum;
        }
    }
}
