
#include "sub.h"

void SubBytes(state_t *state) {
    #pragma HLS INLINE off
    #pragma HLS PIPELINE II=1

    for (uint8_t i = 0; i < 4; i++) {
        #pragma HLS UNROLL
        for (uint8_t j = 0; j < 4; j++) {
            #pragma HLS UNROLL
            (*state)[i][j] = getSBoxValue((*state)[i][j]);
        }
    }
}
