
#include "shift_rows.h"

void ShiftRows(state_t *state) {
    uint8_t temp;

    // Row 0 is not shifted.

    // Row 1: shift left by 1.
    temp = (*state)[1][0];
    (*state)[1][0] = (*state)[1][1];
    (*state)[1][1] = (*state)[1][2];
    (*state)[1][2] = (*state)[1][3];
    (*state)[1][3] = temp;

    // Row 2: shift left by 2.
    uint8_t temp0 = (*state)[2][0];
    uint8_t temp1 = (*state)[2][1];
    (*state)[2][0] = (*state)[2][2];
    (*state)[2][1] = (*state)[2][3];
    (*state)[2][2] = temp0;
    (*state)[2][3] = temp1;

    // Row 3: shift left by 3.
    temp = (*state)[3][3];
    (*state)[3][3] = (*state)[3][2];
    (*state)[3][2] = (*state)[3][1];
    (*state)[3][1] = (*state)[3][0];
    (*state)[3][0] = temp;
}
