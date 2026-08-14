
#include "gemm_blocked.h"

void bbgemm(TYPE m1[N], TYPE m2[N], TYPE prod[N]) {
    int i, j, k, jj, kk;
    TYPE sum;

    // Initialize output matrix to zero
    for (i = 0; i < row_size; i++) {
        for (j = 0; j < col_size; j++) {
            prod[i * col_size + j] = 0.0;
        }
    }

    // Blocked matrix multiplication
    for (jj = 0; jj < row_size; jj += block_size) {
        for (kk = 0; kk < row_size; kk += block_size) {
            for (i = 0; i < row_size; i++) {
                for (k = kk; k < kk + block_size; k++) {
                    sum = 0.0;
                    for (j = jj; j < jj + block_size; j++) {
                        sum += m1[i * col_size + k] * m2[k * col_size + j];
                        prod[i * col_size + j] += sum;
                    }
                }
            }
        }
    }
}
