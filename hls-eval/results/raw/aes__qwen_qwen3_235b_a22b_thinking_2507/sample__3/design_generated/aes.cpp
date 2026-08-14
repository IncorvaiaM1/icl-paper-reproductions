
#include "aes.h"

static void KeyExpansion(uint8_t *RoundKey, const uint8_t *Key) {
    for (int i = 0; i < 16; i++) {
        RoundKey[i] = Key[i];
    }

    int i = 16;
    uint8_t temp[4];
    while (i < 176) {
        for (int t = 0; t < 4; t++) {
            temp[t] = RoundKey[i-4+t];
        }

        if (i % 16 == 0) {
            uint8_t t0 = temp[0];
            temp[0] = temp[1];
            temp[1] = temp[2];
            temp[2] = temp[3];
            temp[3] = t0;

            for (int t = 0; t < 4; t++) {
                temp[t] = sbox[temp[t]];
            }

            temp[0] ^= Rcon[i/16];
        }

        for (int t = 0; t < 4; t++) {
            RoundKey[i] = RoundKey[i-16] ^ temp[t];
            i++;
        }
    }
}

void AES_init_ctx(struct AES_ctx *ctx, const uint8_t *key) {
    KeyExpansion(ctx->RoundKey, key);
}

static inline uint8_t xtime(uint8_t x) {
    return (x << 1) ^ ((x & 0x80) ? 0x1b : 0x00);
}

static inline uint8_t mul2(uint8_t x) {
    return xtime(x);
}

static inline uint8_t mul3(uint8_t x) {
    return xtime(x) ^ x;
}

static void SubBytes(state_t *state) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            (*state)[i][j] = sbox[(*state)[i][j]];
        }
    }
}

static void ShiftRows(state_t *state) {
    uint8_t t;
    
    t = (*state)[1][0];
    (*state)[1][0] = (*state)[1][1];
    (*state)[1][1] = (*state)[1][2];
    (*state)[1][2] = (*state)[1][3];
    (*state)[1][3] = t;
    
    t = (*state)[2][0];
    (*state)[2][0] = (*state)[2][2];
    (*state)[2][2] = t;
    t = (*state)[2][1];
    (*state)[2][1] = (*state)[2][3];
    (*state)[2][3] = t;
    
    t = (*state)[3][3];
    (*state)[3][3] = (*state)[3][2];
    (*state)[3][2] = (*state)[3][1];
    (*state)[3][1] = (*state)[3][0];
    (*state)[3][0] = t;
}

static void MixColumns(state_t *state) {
    for (int j = 0; j < 4; j++) {
        uint8_t a[4];
        a[0] = (*state)[0][j];
        a[1] = (*state)[1][j];
        a[2] = (*state)[2][j];
        a[3] = (*state)[3][j];
        
        (*state)[0][j] = mul2(a[0]) ^ mul3(a[1]) ^ a[2] ^ a[3];
        (*state)[1][j] = a[0] ^ mul2(a[1]) ^ mul3(a[2]) ^ a[3];
        (*state)[2][j] = a[0] ^ a[1] ^ mul2(a[2]) ^ mul3(a[3]);
        (*state)[3][j] = mul3(a[0]) ^ a[1] ^ a[2] ^ mul2(a[3]);
    }
}

static void AddRoundKey(uint8_t round, state_t *state, const round_t *RoundKey) {
    uint8_t *rk = (uint8_t*)RoundKey + round * 16;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            (*state)[i][j] ^= rk[i*4+j];
        }
    }
}

void Cipher(state_t *state, const round_t *RoundKey) {
    AddRoundKey(0, state, RoundKey);
    
    for (int round = 1; round < 10; round++) {
        SubBytes(state);
        ShiftRows(state);
        MixColumns(state);
        AddRoundKey(round, state, RoundKey);
    }
    
    SubBytes(state);
    ShiftRows(state);
    AddRoundKey(10, state, RoundKey);
}
