
#include "shift_rows.h"

void ShiftRows(state_t *state) {
    #pragma HLS INTERFACE m_axi port=state depth=16
    #pragma HLS INTERFACE s_axilite port=return

    state_t temp_state;
    
    // Copy the original state to temporary state
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            temp_state[i][j] = (*state)[i][j];
        }
    }

    // Perform ShiftRows operation
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            (*state)[i][j] = temp_state[i][(j + i) % 4];
        }
    }
}
