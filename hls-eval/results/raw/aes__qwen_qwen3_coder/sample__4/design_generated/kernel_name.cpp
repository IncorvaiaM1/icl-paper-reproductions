
#include "aes.h"

void AddRoundKey(uint8_t round, state_t *state, const round_t *RoundKey) {
    uint8_t i, j;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            (*state)[i][j] ^= (*RoundKey)[round * Nb * 4 + i * Nb + j];
        }
    }
}

void SubBytes(state_t *state) {
    uint8_t i, j;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            (*state)[i][j] = getSBoxValue((*state)[i][j]);
        }
    }
}

void ShiftRows(state_t *state) {
    uint8_t temp;

    // Row 1: shift left by 1
    temp = (*state)[1][0];
    (*state)[1][0] = (*state)[1][1];
    (*state)[1][1] = (*state)[1][2];
    (*state)[1][2] = (*state)[1][3];
    (*state)[1][3] = temp;

    // Row 2: shift left by 2
    temp = (*state)[2][0];
    (*state)[2][0] = (*state)[2][2];
    (*state)[2][2] = temp;
    temp = (*state)[2][1];
    (*state)[2][1] = (*state)[2][3];
    (*state)[2][3] = temp;

    // Row 3: shift left by 3 (or right by 1)
    temp = (*state)[3][0];
    (*state)[3][0] = (*state)[3][3];
    (*state)[3][3] = (*state)[3][2];
    (*state)[3][2] = (*state)[3][1];
    (*state)[3][1] = temp;
}

// xtime operation for MixColumns
uint8_t xtime(uint8_t x) {
    return (x << 1) ^ (((x >> 7) & 1) * 0x1b);
}

void MixColumns(state_t *state) {
    uint8_t i;
    for (i = 0; i < 4; ++i) {
        uint8_t s0 = (*state)[0][i];
        uint8_t s1 = (*state)[1][i];
        uint8_t s2 = (*state)[2][i];
        uint8_t s3 = (*state)[3][i];

        uint8_t h0 = s0 ^ s1 ^ s2 ^ s3;
        uint8_t h1 = s0 ^ s1;
        h1 = xtime(h1) ^ h0;
        uint8_t h2 = s1 ^ s2;
        h2 = xtime(h2) ^ h0;
        uint8_t h3 = s2 ^ s3;
        h3 = xtime(h3) ^ h0;

        (*state)[0][i] = s0 ^ h1;
        (*state)[1][i] = s1 ^ h2;
        (*state)[2][i] = s2 ^ h3;
        (*state)[3][i] = s3 ^ h1;
    }
}

void Cipher(state_t *state, const round_t *RoundKey) {
    uint8_t round;

    // Initial round
    AddRoundKey(0, state, RoundKey);

    // Main rounds
    for (round = 1; round < Nr; ++round) {
        SubBytes(state);
        ShiftRows(state);
        MixColumns(state);
        AddRoundKey(round, state, RoundKey);
    }

    // Final round
    SubBytes(state);
    ShiftRows(state);
    AddRoundKey(Nr, state, RoundKey);
}

void AES_init_ctx(struct AES_ctx *ctx, const uint8_t *key) {
    // Key expansion is assumed to be done externally or in a separate function
    // For this task, we just copy the key into the context
    for (int i = 0; i < AES_keyExpSize; ++i) {
        ctx->RoundKey[i] = key[i % AES_KEYLEN];
    }
}
