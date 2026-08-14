
#include "monobit.h"

void Frequency(int *result, int epsilon[N]) {
    #pragma HLS INTERFACE m_axi port=result offset=slave bundle=gmem
    #pragma HLS INTERFACE m_axi port=epsilon offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=result bundle=control
    #pragma HLS INTERFACE s_axilite port=epsilon bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control

    int sum = 0;

    for (int i = 0; i < N; i++) {
        #pragma HLS PIPELINE II=1
        sum += 2 * epsilon[i] - 1;
    }

    *result = sum;
}
