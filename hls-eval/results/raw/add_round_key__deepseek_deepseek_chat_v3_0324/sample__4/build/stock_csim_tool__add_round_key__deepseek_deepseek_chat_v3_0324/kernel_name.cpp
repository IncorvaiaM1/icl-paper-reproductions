
#include "add_round_key.h"

void AddRoundKey(uint8_t round, state_t *state, const uint8_t *RoundKey) {
    #pragma HLS INTERFACE ap_ctrl_none port=return
    #pragma HLS INTERFACE ap_none port=state
    #pragma HLS INTERFACE ap_none port=RoundKey
    #pragma HLS INTERFACE ap_none port=round
    
    for (int i = 0; i < 4; i++) {
        #pragma HLS UNROLL
        for (int j = 0; j < 4; j++) {
            #pragma HLS UNROLL
            (*state)[i][j] ^= RoundKey[(round * Nb * 4) + (i * Nb) + j];
        }
    }
}
