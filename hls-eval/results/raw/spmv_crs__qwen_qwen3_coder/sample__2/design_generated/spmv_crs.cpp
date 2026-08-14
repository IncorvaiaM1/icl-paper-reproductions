
#include "spmv_crs.h"

void spmv(TYPE val[NNZ], int32_t cols[NNZ], int32_t rowDelimiters[N + 1], TYPE vec[N], TYPE out[N]) {
    int i, j;
    TYPE sum;
    int tmp_begin, tmp_end;

    // Iterate over each row of the sparse matrix
    for (i = 0; i < N; i++) {
        sum = 0.0;
        tmp_begin = rowDelimiters[i];
        tmp_end = rowDelimiters[i + 1];

        // Iterate over non-zero elements in the current row
        for (j = tmp_begin; j < tmp_end; j++) {
            sum += val[j] * vec[cols[j]];
        }

        // Store the result for the current row
        out[i] = sum;
    }
}
