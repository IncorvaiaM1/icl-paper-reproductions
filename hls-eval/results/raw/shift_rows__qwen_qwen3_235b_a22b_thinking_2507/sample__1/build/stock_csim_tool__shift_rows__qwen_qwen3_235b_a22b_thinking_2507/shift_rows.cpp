
#include "shift_rows.h"

void ShiftRows(state_t *state) {
    // Shift row 1 (index 1) left by 1 position
    {
        uint8_t t = (*state)[1][0];
        (*state)[1][0] = (*state)[1][1];
        (*state)[1][1] = (*state)[1][2];
        (*state)[1][2] = (*state)[1][3];
        (*state)[1][3] = t;
    }
    
    // Shift row 2 (index 2) left by 2 positions
    {
        uint8_t t0 = (*state)[2][0];
        uint8_t t1 = (*state)[2][1];
        (*state)[2][0] = (*state)[2][2];
        (*state)[2][1] = (*state)[2][3];
        (*state)[2][2] = t0;
        (*state)[2][3] = t1;
    }
    
    // Shift row 3 (index 3) left by 3 positions
    {
        uint8_t t = (*state)[3][0];
        (*state)[3][0] = (*state)[3][3];
        (*state)[3][3] = (*state)[3][2];
        (*state)[3][2] = (*state)[3][1];
        (*state)[3][1] = t;
    }
}
