
#include "mix_cols.h"

uint8_t xtime(uint8_t x) {
    return (x << 1) ^ ((x >> 7) & 1) * 0x1b;
}

void MixColumns(state_t *state) {
    for (int col = 0; col < 4; ++col) {
        uint8_t a0 = (*state)[col][0];
        uint8_t a1 = (*state)[col][1];
        uint8_t a2 = (*state)[col][2];
        uint8_t a3 = (*state)[col][3];
        
        (*state)[col][0] = xtime(a0) ^ (xtime(a1) ^ a1) ^ a2 ^ a3;
        (*state)[col][1] = a0 ^ xtime(a1) ^ (xtime(a2) ^ a2) ^ a3;
        (*state)[col][2] = a0 ^ a1 ^ xtime(a2) ^ (xtime(a3) ^ a3);
        (*state)[col][3] = (xtime(a0) ^ a0) ^ a1 ^ a2 ^ xtime(a3);
    }
}
