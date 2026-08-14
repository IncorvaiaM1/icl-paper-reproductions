
#include "gemm_blocked.h"

void bbgemm(TYPE m1[N], TYPE m2[N], TYPE prod[N]) {
    // Initialize output matrix to zero
    for (int i = 0; i < N; i++) {
        prod[i] = 0;
    }

    // Blocked matrix multiplication
    for (int jj = 0; jj < row_size; jj += block_size) {
        for (int kk = 0; kk < col_size; kk += block_size) {
            for (int i = 0; i < row_size; i++) {
                for (int k = kk; k < kk + block_size; k++) {
                    TYPE sum = prod[i * row_size + jj];
                    for (int j = jj; j < jj + block_size; j++) {
                        sum += m1[i * row_size + k] * m2[k * row_size + j];
                    }
                    prod[i * row_size + jj] = sum;
                }
            }
        }
    }
}
