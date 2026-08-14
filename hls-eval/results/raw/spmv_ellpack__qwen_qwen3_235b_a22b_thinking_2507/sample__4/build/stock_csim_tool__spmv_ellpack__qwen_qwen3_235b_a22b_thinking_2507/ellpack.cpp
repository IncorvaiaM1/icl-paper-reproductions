
#include "spmv_ellpack.h"

void ellpack(TYPE nzval[N * L], int32_t cols[N * L], TYPE vec[N], TYPE out[N]) {
ellpack_1: for (int i = 0; i < N; i++) {
        out[i] = 0.0;
ellpack_2: for (int j = 0; j < L; j++) {
            #pragma HLS UNROLL
            int index = i * L + j;
            out[i] += nzval[index] * vec[cols[index]];
        }
    }
}
