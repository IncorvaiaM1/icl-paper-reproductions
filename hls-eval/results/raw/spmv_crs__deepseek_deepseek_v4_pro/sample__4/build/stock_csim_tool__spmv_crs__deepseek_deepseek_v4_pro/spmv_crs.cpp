
#include "spmv_crs.h"

void spmv(TYPE val[NNZ], int32_t cols[NNZ], int32_t rowDelimiters[N + 1], TYPE vec[N], TYPE out[N]) {
    for (int i = 0; i != N; ++i) {
        TYPE sum = 0.0;
        int32_t begin = rowDelimiters[i];
        int32_t end = rowDelimiters[i + 1];
        for (int32_t j = begin; j != end; ++j) {
            sum += val[j] * vec[cols[j]];
        }
        out[i] = sum;
    }
}
