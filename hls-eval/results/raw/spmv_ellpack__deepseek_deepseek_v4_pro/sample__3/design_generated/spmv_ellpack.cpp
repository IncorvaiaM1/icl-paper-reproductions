
#include "spmv_ellpack.h"

void ellpack(TYPE nzval[N * L], int32_t cols[N * L], TYPE vec[N], TYPE out[N]) {
    int i, j;
    TYPE sum;

    for (i = 0; i < N; i++) {
        sum = (TYPE)0;

        for (j = 0; j < L; j++) {
            int32_t col = cols[i * L + j];
            sum += nzval[i * L + j] * vec[col];
        }

        out[i] = sum;
    }
}
