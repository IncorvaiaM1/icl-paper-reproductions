
#include "gemm_blocked.h"

void bbgemm(TYPE m1[N], TYPE m2[N], TYPE prod[N]) {
    // Initialize output matrix to zero
    for (int i = 0; i < row_size; i++) {
        for (int j = 0; j < col_size; j++) {
            prod[i * col_size + j] = 0;
        }
    }

    // Blocked GEMM implementation with cache-optimized blocking
    for (int jj = 0; jj < col_size; jj += block_size) {
        for (int kk = 0; kk < row_size; kk += block_size) {
            for (int ii = 0; ii < row_size; ii += block_size) {
                for (int i = ii; i < ii + block_size; i++) {
                    for (int k = kk; k < kk + block_size; k++) {
                        for (int j = jj; j < jj + block_size; j++) {
                            // Accumulate partial products for matrix multiplication
                            prod[i * col_size + j] += m1[i * col_size + k] * m2[k * col_size + j];
                        }
                    }
                }
            }
        }
    }
}
