
#include "shift_rows.h"

void ShiftRows(state_t *state) {
    state_t &s = *state;

    // Row 1: left rotate by 1
    uint8_t temp = s[1][0];
    s[1][0] = s[1][1];
    s[1][1] = s[1][2];
    s[1][2] = s[1][3];
    s[1][3] = temp;

    // Row 2: left rotate by 2
    uint8_t temp0 = s[2][0];
    uint8_t temp1 = s[2][1];
    s[2][0] = s[2][2];
    s[2][1] = s[2][3];
    s[2][2] = temp0;
    s[2][3] = temp1;

    // Row 3: left rotate by 3 (implemented as right rotate by 1)
    temp = s[3][3];
    s[3][3] = s[3][2];
    s[3][2] = s[3][1];
    s[3][1] = s[3][0];
    s[3][0] = temp;
}
