
#include "gemm_ncubed.h"

void gemm(TYPE m1[N], TYPE m2[N], TYPE prod[N]) {
    // Outer loop: iterate over rows of the output matrix
    for (int i = 0; i < row_size; i++) {
        // Middle loop: iterate over columns of the output matrix
        for (int j = 0; j < col_size; j++) {
            TYPE sum = 0.0;

            // Inner loop: accumulate products over the common dimension
            for (int k = 0; k < row_size; k++) {
                sum += m1[i * row_size + k] * m2[k * row_size + j];
            }

            prod[i * row_size + j] = sum;
        }
    }
}
