
#include "aes.h"

static uint8_t xtime(uint8_t x) {
    return ((x << 1) ^ (((x >> 7) & 1) * 0x1b));
}

void SubBytes(state_t *state) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            (*state)[i][j] = getSBoxValue((*state)[i][j]);
        }
    }
}

void ShiftRows(state_t *state) {
    uint8_t temp;

    // Rotate first row 1 columns to left
    temp = (*state)[0][1];
    (*state)[0][1] = (*state)[1][1];
    (*state)[1][1] = (*state)[2][1];
    (*state)[2][1] = (*state)[3][1];
    (*state)[3][1] = temp;

    // Rotate second row 2 columns to left
    temp = (*state)[0][2];
    (*state)[0][2] = (*state)[2][2];
    (*state)[2][2] = temp;

    temp = (*state)[1][2];
    (*state)[1][2] = (*state)[3][2];
    (*state)[3][2] = temp;

    // Rotate third row 3 columns to left
    temp = (*state)[0][3];
    (*state)[0][3] = (*state)[3][3];
    (*state)[3][3] = (*state)[2][3];
    (*state)[2][3] = (*state)[1][3];
    (*state)[1][3] = temp;
}

void MixColumns(state_t *state) {
    uint8_t tmp, tm, t;
    for (int i = 0; i < 4; ++i) {
        t = (*state)[i][0];
        tmp = (*state)[i][0] ^ (*state)[i][1] ^ (*state)[i][2] ^ (*state)[i][3];
        tm = (*state)[i][0] ^ (*state)[i][1];
        tm = xtime(tm);
        (*state)[i][0] ^= tm ^ tmp;
        tm = (*state)[i][1] ^ (*state)[i][2];
        tm = xtime(tm);
        (*state)[i][1] ^= tm ^ tmp;
        tm = (*state)[i][2] ^ (*state)[i][3];
        tm = xtime(tm);
        (*state)[i][2] ^= tm ^ tmp;
        tm = (*state)[i][3] ^ t;
        tm = xtime(tm);
        (*state)[i][3] ^= tm ^ tmp;
    }
}

void AddRoundKey(uint8_t round, state_t *state, const round_t *RoundKey) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            (*state)[i][j] ^= (*RoundKey)[(round * Nb * 4) + (i * Nb) + j];
        }
    }
}

void KeyExpansion(uint8_t *RoundKey, const uint8_t *Key) {
    unsigned i, j, k;
    uint8_t tempa[4];

    for (i = 0; i < Nk; ++i) {
        RoundKey[(i * 4) + 0] = Key[(i * 4) + 0];
        RoundKey[(i * 4) + 1] = Key[(i * 4) + 1];
        RoundKey[(i * 4) + 2] = Key[(i * 4) + 2];
        RoundKey[(i * 4) + 3] = Key[(i * 4) + 3];
    }

    for (i = Nk; i < Nb * (Nr + 1); ++i) {
        k = (i - 1) * 4;
        tempa[0] = RoundKey[k + 0];
        tempa[1] = RoundKey[k + 1];
        tempa[2] = RoundKey[k + 2];
        tempa[3] = RoundKey[k + 3];

        if (i % Nk == 0) {
            const uint8_t u8tmp = tempa[0];
            tempa[0] = tempa[1];
            tempa[1] = tempa[2];
            tempa[2] = tempa[3];
            tempa[3] = u8tmp;

            tempa[0] = getSBoxValue(tempa[0]);
            tempa[1] = getSBoxValue(tempa[1]);
            tempa[2] = getSBoxValue(tempa[2]);
            tempa[3] = getSBoxValue(tempa[3]);

            tempa[0] ^= Rcon[i / Nk];
        }

        j = i * 4;
        k = (i - Nk) * 4;
        RoundKey[j + 0] = RoundKey[k + 0] ^ tempa[0];
        RoundKey[j + 1] = RoundKey[k + 1] ^ tempa[1];
        RoundKey[j + 2] = RoundKey[k + 2] ^ tempa[2];
        RoundKey[j + 3] = RoundKey[k + 3] ^ tempa[3];
    }
}

void AES_init_ctx(struct AES_ctx *ctx, const uint8_t *key) {
    KeyExpansion(ctx->RoundKey, key);
}

void Cipher(state_t *state, const round_t *RoundKey) {
    AddRoundKey(0, state, RoundKey);

    for (uint8_t round = 1; round < Nr; ++round) {
        SubBytes(state);
        ShiftRows(state);
        MixColumns(state);
        AddRoundKey(round, state, RoundKey);
    }

    SubBytes(state);
    ShiftRows(state);
    AddRoundKey(Nr, state, RoundKey);
}
