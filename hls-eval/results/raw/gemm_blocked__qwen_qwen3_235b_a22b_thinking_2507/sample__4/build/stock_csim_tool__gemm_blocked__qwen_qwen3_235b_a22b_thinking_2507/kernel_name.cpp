
#include "gemm_blocked.h"

void bbgemm(TYPE m1[N], TYPE m2[N], TYPE prod[N]) {
    // Initialize the entire output matrix to zero
    for (int i = 0; i < N; i++) {
        prod[i] = 0.0;
    }

    // Blocked GEMM implementation with four nested loops for blocking and inner computation
    // loopjj: iterates over column blocks of the output matrix
    // loopkk: iterates over common dimension blocks
    for (int jj = 0; jj < col_size; jj += block_size) {
        // loopi: iterates over rows of input matrix A
        for (int kk = 0; kk < row_size; kk += block_size) {
            for (int i = 0; i < row_size; i++) {
                // loopk: iterates over the common dimension within the current block
                for (int k = kk; k < (kk + block_size) && k < row_size; k++) {
                    // loopj: innermost loop that accumulates partial products (iterates over columns within block)
                    for (int j = jj; j < (jj + block_size) && j < col_size; j++) {
                        // Compute matrix multiplication: C(i,j) += A(i,k) * B(k,j)
                        // Using row-major storage: index = row * col_size + column
                        prod[i * col_size + j] += m1[i * col_size + k] * m2[k * col_size + j];
                    }
                }
            }
        }
    }
}
