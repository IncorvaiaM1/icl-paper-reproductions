
#include "shift_rows.h"

void ShiftRows(state_t *state) {
    // Row 0 remains unchanged
    // Row 1: shift left by 1 (rotate)
    uint8_t temp1 = (*state)[1][0];
    (*state)[1][0] = (*state)[1][1];
    (*state)[1][1] = (*state)[1][2];
    (*state)[1][2] = (*state)[1][3];
    (*state)[1][3] = temp1;

    // Row 2: shift left by 2 (rotate twice)
    uint8_t temp2a = (*state)[2][0];
    uint8_t temp2b = (*state)[2][1];
    (*state)[2][0] = (*state)[2][2];
    (*state)[2][1] = (*state)[2][3];
    (*state)[2][2] = temp2a;
    (*state)[2][3] = temp2b;

    // Row 3: shift left by 3 (rotate thrice)
    uint8_t temp3 = (*state)[3][3];
    (*state)[3][3] = (*state)[3][2];
    (*state)[3][2] = (*state)[3][1];
    (*state)[3][1] = (*state)[3][0];
    (*state)[3][0] = temp3;
}
