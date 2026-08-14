
#include "spmv_ellpack.h"

void ellpack(TYPE nzval[N * L], int32_t cols[N * L], TYPE vec[N], TYPE out[N]) {
    #pragma HLS LOOP_LABEL ellpack_1
    for (int i = 0; i < N; i++) {
        #pragma HLS PIPELINE
        out[i] = 0.0;
        #pragma HLS LOOP_LABEL ellpack_2
        for (int j = 0; j < L; j++) {
            #pragma HLS UNROLL
            int index = i * L + j;
            out[i] += nzval[index] * vec[cols[index]];
        }
    }
}
