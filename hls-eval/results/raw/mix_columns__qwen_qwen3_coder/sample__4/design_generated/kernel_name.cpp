
#include "mix_cols.h"

uint8_t xtime(uint8_t x) {
    return (x << 1) ^ (((x >> 7) & 1) * 0x1b);
}

void MixColumns(state_t *state) {
    uint8_t i, Tmp, Tm, t;

    for (i = 0; i < 4; ++i) {
        t = (*state)[i][0] ^ (*state)[i][1] ^ (*state)[i][2] ^ (*state)[i][3];
        Tmp = (*state)[i][0] ^ (*state)[i][1];
        Tm = (*state)[i][0] ^ (*state)[i][2];
        Tmp = xtime(Tmp);
        (*state)[i][0] ^= Tmp ^ t;
        Tm = xtime(Tm);
        (*state)[i][1] ^= Tm ^ t;
        Tmp ^= Tm;
        (*state)[i][2] ^= Tmp ^ t;
        Tm ^= Tmp;
        (*state)[i][3] ^= Tm ^ t;
    }
}
