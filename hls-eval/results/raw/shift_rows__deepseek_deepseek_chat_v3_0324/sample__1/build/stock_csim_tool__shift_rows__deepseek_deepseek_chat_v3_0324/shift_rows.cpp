
#include "shift_rows.h"

void ShiftRows(state_t *state) {
    #pragma HLS INTERFACE m_axi port=state depth=16
    #pragma HLS INTERFACE s_axilite port=return bundle=control

    uint8_t temp;

    // Row 1: Rotate left by 1 byte
    temp = (*state)[1][0];
    (*state)[1][0] = (*state)[1][1];
    (*state)[1][1] = (*state)[1][2];
    (*state)[1][2] = (*state)[1][3];
    (*state)[1][3] = temp;

    // Row 2: Rotate left by 2 bytes
    temp = (*state)[2][0];
    (*state)[2][0] = (*state)[2][2];
    (*state)[2][2] = temp;
    temp = (*state)[2][1];
    (*state)[2][1] = (*state)[2][3];
    (*state)[2][3] = temp;

    // Row 3: Rotate left by 3 bytes (equivalent to rotating right by 1 byte)
    temp = (*state)[3][3];
    (*state)[3][3] = (*state)[3][2];
    (*state)[3][2] = (*state)[3][1];
    (*state)[3][1] = (*state)[3][0];
    (*state)[3][0] = temp;
}
