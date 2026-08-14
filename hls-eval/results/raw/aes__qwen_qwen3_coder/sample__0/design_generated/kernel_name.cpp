
#include "aes.h"

void AddRoundKey(uint8_t round, state_t *state, const uint8_t *RoundKey) {
    uint8_t i, j;
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            (*state)[i][j] ^= RoundKey[(round * Nb * 4) + (i * Nb) + j];
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

    // Row 0 - No shift

    // Row 1 - Shift left by 1
    temp = (*state)[1][0];
    (*state)[1][0] = (*state)[1][1];
    (*state)[1][1] = (*state)[1][2];
    (*state)[1][2] = (*state)[1][3];
    (*state)[1][3] = temp;

    // Row 2 - Shift left by 2
    temp = (*state)[2][0];
    (*state)[2][0] = (*state)[2][2];
    (*state)[2][2] = temp;
    temp = (*state)[2][1];
    (*state)[2][1] = (*state)[2][3];
    (*state)[2][3] = temp;

    // Row 3 - Shift left by 3 (or right by 1)
    temp = (*state)[3][3];
    (*state)[3][3] = (*state)[3][2];
    (*state)[3][2] = (*state)[3][1];
    (*state)[3][1] = (*state)[3][0];
    (*state)[3][0] = temp;
}

// MixColumns uses a Galois field multiplier, simplified here for clarity.
// xtime(x) = x*2 (in GF(2^8))
uint8_t xtime(uint8_t x) {
    return ((x << 1) ^ (((x >> 7) & 1) * 0x1b));
}

void MixColumns(state_t *state) {
    uint8_t i;
    for (i = 0; i < 4; ++i) {
        uint8_t Tmp, Tm, t;
        Tmp = (*state)[i][0] ^ (*state)[i][1] ^ (*state)[i][2] ^ (*state)[i][3];
        Tm = (*state)[i][0] ^ (*state)[i][1];
        t = xtime(Tm);
        (*state)[i][0] ^= t ^ Tmp;
        Tm = (*state)[i][1] ^ (*state)[i][2];
        t = xtime(Tm);
        (*state)[i][1] ^= t ^ Tmp;
        Tm = (*state)[i][2] ^ (*state)[i][3];
        t = xtime(Tm);
        (*state)[i][2] ^= t ^ Tmp;
        Tm = (*state)[i][3] ^ (*state)[i][0];
        t = xtime(Tm);
        (*state)[i][3] ^= t ^ Tmp;
    }
}

void Cipher(state_t *state, const uint8_t *RoundKey) {
    uint8_t round;

    // Initial round key addition
    AddRoundKey(0, state, RoundKey);

    // Main rounds
    for (round = 1; round < Nr; ++round) {
        SubBytes(state);
        ShiftRows(state);
        MixColumns(state);
        AddRoundKey(round, state, RoundKey);
    }

    // Final round (no MixColumns)
    SubBytes(state);
    ShiftRows(state);
    AddRoundKey(Nr, state, RoundKey);
}

void AES_init_ctx(struct AES_ctx *ctx, const uint8_t *key) {
    // Key expansion is assumed to be done externally or by another function.
    // For simplicity in HLS context, we assume the key is already expanded.
    // This function is just a placeholder as per the header.
}
