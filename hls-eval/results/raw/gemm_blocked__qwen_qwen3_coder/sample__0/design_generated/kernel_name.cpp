
#include "gemm_blocked.h"

void bbgemm(TYPE m1[N], TYPE m2[N], TYPE prod[N]) {
    int jj, kk, i, j, k;
    TYPE accum[block_size][block_size];
    TYPE b1[block_size][block_size];
    TYPE b2[block_size][block_size];
    
    // Initialize output matrix to zero
    for (i = 0; i < N; i++) {
        prod[i] = 0.0;
    }

    // Loop over block columns
    for (jj = 0; jj < col_size; jj += block_size) {
        // Loop over block rows
        for (kk = 0; kk < row_size; kk += block_size) {
            // Load block of m2 into local storage
            for (i = 0; i < block_size; i++) {
                for (j = 0; j < block_size; j++) {
                    b2[i][j] = m2[(kk + i) * col_size + (jj + j)];
                }
            }
            
            // Loop over block rows of m1
            for (i = 0; i < row_size; i += block_size) {
                // Load block of m1 into local storage
                for (k = 0; k < block_size; k++) {
                    for (j = 0; j < block_size; j++) {
                        b1[k][j] = m1[(i + k) * col_size + (kk + j)];
                    }
                }
                
                // Initialize accumulator
                for (k = 0; k < block_size; k++) {
                    for (j = 0; j < block_size; j++) {
                        accum[k][j] = 0.0;
                    }
                }
                
                // Perform block multiplication
                for (k = 0; k < block_size; k++) {
                    for (j = 0; j < block_size; j++) {
                        for (int l = 0; l < block_size; l++) {
                            accum[k][j] += b1[k][l] * b2[l][j];
                        }
                    }
                }
                
                // Accumulate result to output matrix
                for (k = 0; k < block_size; k++) {
                    for (j = 0; j < block_size; j++) {
                        prod[(i + k) * col_size + (jj + j)] += accum[k][j];
                    }
                }
            }
        }
    }
}
