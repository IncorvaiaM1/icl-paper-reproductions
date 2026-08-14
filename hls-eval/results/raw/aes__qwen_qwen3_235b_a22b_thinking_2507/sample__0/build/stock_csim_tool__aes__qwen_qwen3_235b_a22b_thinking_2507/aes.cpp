
#include "aes.h"

static inline uint8_t xtime(uint8_t x) {
    return (x << 1) ^ (0x1b * (x >> 7));
}

static void SubBytes(state_t *state) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            (*state)[i][j] = sbox[(*state)[i][j]];
        }
    }
}

static void ShiftRows(state_t *state) {
    uint8_t temp;
    temp = (*state)[1][0];
    (*state)[1][0] = (*state)[1][1];
    (*state)[1][1] = (*state)[1][2];
    (*state)[1][2] = (*state)[1][3];
    (*state)[1][3] = temp;

    temp = (*state)[2][0];
    (*state)[2][0] = (*state)[2][2];
    (*state)[2][2] = temp;
    temp = (*state)[2][1];
    (*state)[2][1] = (*state)[2][3];
    (*state)[2][3] = temp;

    temp = (*state)[3][3];
    (*state)[3][3] = (*state)[3][2];
    (*state)[3][2] = (*state)[3][1];
    (*state)[3][1] = (*state)[3][0];
    (*state)[3][0] = temp;
}

static void MixColumns(state_t *state) {
    for (int j = 0; j < 4; j++) {
        uint8_t s0 = (*state)[0][j];
        uint8_t s1 = (*state)[1][j];
        uint8_t s2 = (*state)[2][j];
        uint8_t s3 = (*state)[3][j];

        uint8_t t0 = xtime(s0) ^ xtime(s1) ^ s1 ^ s2 ^ s3;
        uint8_t t1 = s0 ^ xtime(s1) ^ xtime(s2) ^ s2 ^ s3;
        uint8_t t2 = s0 ^ s1 ^ xtime(s2) ^ xtime(s3) ^ s3;
        uint8_t t3 = xtime(s0) ^ s0 ^ s1 ^ s2 ^ xtime(s3);

        (*state)[0][j] = t0;
        (*state)[1][j] = t1;
        (*state)[2][j] = t2;
        (*state)[3][j] = t3;
    }
}

static void AddRoundKey(uint8_t round, state_t *state, const round_t *RoundKey) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            (*state)[i][j] ^= (*RoundKey)[round * 16 + 4 * j + i];
        }
    }
}

void Cipher(state_t *state, const round_t *RoundKey) {
    AddRoundKey(0, state, RoundKey);

    for (uint8_t round = 1; round < 10; round++) {
        SubBytes(state);
        ShiftRows(state);
        MixColumns(state);
        AddRoundKey(round, state, RoundKey);
    }

    SubBytes(state);
    ShiftRows(state);
    AddRoundKey(10, state, RoundKey);
}
