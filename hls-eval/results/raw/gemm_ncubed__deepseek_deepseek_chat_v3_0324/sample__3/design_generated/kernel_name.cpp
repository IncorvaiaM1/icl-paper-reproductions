
#include "gemm_ncubed.h"

void gemm(double m1[4096], double m2[4096], double prod[4096]) {
    // Initialize output matrix to zero
    for (int i = 0; i < N; i++) {
        prod[i] = 0.0;
    }

    // Perform matrix multiplication
    for (int i = 0; i < row_size; i++) {
        for (int j = 0; j < col_size; j++) {
            double sum = 0.0;
            for (int k = 0; k < col_size; k++) {
                sum += m1[i * col_size + k] * m2[k * col_size + j];
            }
            prod[i * col_size + j] = sum;
        }
    }
}
