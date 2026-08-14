
#include "spmv_crs.h"

void spmv(
    TYPE val[NNZ],
    int32_t cols[NNZ],
    int32_t rowDelimiters[N + 1],
    TYPE vec[N],
    TYPE out[N]) {
    for (int i = 0; i < N; i++) {
        TYPE sum = 0.0;
        int32_t tmp_begin = rowDelimiters[i];
        int32_t tmp_end = rowDelimiters[i + 1];

        for (int32_t j = tmp_begin; j < tmp_end; j++) {
            sum += val[j] * vec[cols[j]];
        }

        out[i] = sum;
    }
}
