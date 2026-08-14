
#include "shift_rows.h"

void ShiftRows(state_t *state) {
    // Row 0 is not shifted.

    // Row 1: shift left by 1.
    uint8_t tmp = (*state)[1][0];
    (*state)[1][0] = (*state)[1][1];
    (*state)[1][1] = (*state)[1][2];
    (*state)[1][2] = (*state)[1][3];
    (*state)[1][3] = tmp;

    // Row 2: shift left by 2.
    tmp = (*state)[2][0];
    (*state)[2][0] = (*state)[2][2];
    (*state)[2][2] = tmp;

    tmp = (*state)[2][1];
    (*state)[2][1] = (*state)[2][3];
    (*state)[2][3] = tmp;

    // Row 3: shift left by 3 (equivalent to shift right by 1).
    tmp = (*state)[3][3];
    (*state)[3][3] = (*state)[3][2];
    (*state)[3][2] = (*state)[3][1];
    (*state)[3][1] = (*state)[3][0];
    (*state)[3][0] = tmp;
}
