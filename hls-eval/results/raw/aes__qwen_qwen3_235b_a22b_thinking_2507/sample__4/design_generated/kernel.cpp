
#include "aes.h"

static inline uint8_t xtime(uint8_t x) {
    return (x << 1) ^ ((x & 0x80) ? 0x1b : 0x00);
}

static void SubBytes(state_t *state) {
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            (*state)[r][c] = sbox[(*state)[r][c]];
        }
    }
}

static void ShiftRows(state_t *state) {
    // Row 1: shift left by 1
    {
        uint8_t t = (*state)[1][0];
        (*state)[1][0] = (*state)[1][1];
        (*state)[1][1] = (*state)[1][2];
        (*state)[1][2] = (*state)[1][3];
        (*state)[1][3] = t;
    }
    // Row 2: shift left by 2
    {
        uint8_t t0 = (*state)[2][0];
        uint8_t t1 = (*state)[2][1];
        (*state)[2][0] = (*state)[2][2];
        (*state)[2][1] = (*state)[2][3];
        (*state)[2][2] = t0;
        (*state)[2][3] = t1;
    }
    // Row 3: shift left by 3 (equivalent to shift right by 1)
    {
        uint8_t t = (*state)[3][3];
        (*state)[3][3] = (*state)[3][2];
        (*state)[3][2] = (*state)[3][1];
        (*state)[3][1] = (*state)[3][0];
        (*state)[3][0] = t;
    }
}

static void MixColumns(state_t *state) {
    for (int c = 0; c < 4; c++) {
        uint8_t s0 = (*state)[0][c];
        uint8_t s1 = (*state)[1][c];
        uint8_t s2 = (*state)[2][c];
        uint8_t s3 = (*state)[3][c];

        uint8_t x0 = xtime(s0);
        uint8_t x1 = xtime(s1);
        uint8_t x2 = xtime(s2);
        uint8_t x3 = xtime(s3);

        (*state)[0][c] = x0 ^ x1 ^ s1 ^ s2 ^ s3;
        (*state)[1][c] = s0 ^ x1 ^ x2 ^ s2 ^ s3;
        (*state)[2][c] = s0 ^ s1 ^ x2 ^ x3 ^ s3;
        (*state)[3][c] = x0 ^ s0 ^ s1 ^ s2 ^ x3;
    }
}

static void AddRoundKey(uint8_t round, state_t *state, const round_t *RoundKey) {
    for (int c = 0; c < 4; c++) {
        for (int r = 0; r < 4; r++) {
            (*state)[r][c] ^= (*RoundKey)[round * 16 + c * 4 + r];
        }
    }
}

void Cipher(state_t *state, const round_t *RoundKey) {
    // Initial round key addition
    AddRoundKey(0, state, RoundKey);
    
    // Rounds 1 to 9: SubBytes, ShiftRows, MixColumns, AddRoundKey
    for (uint8_t round = 1; round < 10; round++) {
        SubBytes(state);
        ShiftRows(state);
        MixColumns(state);
        AddRoundKey(round, state, RoundKey);
    }
    
    // Final round (10): SubBytes, ShiftRows, AddRoundKey (no MixColumns)
    SubBytes(state);
    ShiftRows(state);
    AddRoundKey(10, state, RoundKey);
}
