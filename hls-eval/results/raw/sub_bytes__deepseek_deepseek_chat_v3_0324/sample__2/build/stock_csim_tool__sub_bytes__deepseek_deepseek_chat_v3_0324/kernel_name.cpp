
#include "sub.h"

void SubBytes(state_t *state) {
    #pragma HLS INLINE off
    #pragma HLS PIPELINE II=1
    
    SubBytes_loop_row:
    for (int i = 0; i < 4; ++i) {
        #pragma HLS UNROLL factor=4
        SubBytes_loop_col:
        for (int j = 0; j < 4; ++j) {
            #pragma HLS UNROLL
            (*state)[i][j] = getSBoxValue((*state)[i][j]);
        }
    }
}
