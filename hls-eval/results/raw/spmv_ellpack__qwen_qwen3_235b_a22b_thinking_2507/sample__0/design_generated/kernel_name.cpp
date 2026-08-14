
#include "spmv_ellpack.h"

void ellpack(TYPE nzval[N * L], int32_t cols[N * L], TYPE vec[N], TYPE out[N]) {
    int i, j;
    // ellpack_1 loop: iterate over each row
    for (i = 0; i < N; i++) {
        TYPE sum = 0;
        // ellpack_2 loop: compute dot product for each row
        for (j = 0; j < L; j++) {
            int idx = i * L + j;
            sum += nzval[idx] * vec[cols[idx]];
        }
        out[i] = sum;
    }
}
