
#include "aes_aes.h"

// Galois field logarithm calculation
uint8_t gf_log(uint8_t x) {
    uint8_t y = 1;
    uint8_t i = 0;
    if (x == 0) return 0; // Special case for 0
    while (y != x && i < 255) {
        y = (y << 1) ^ (y >= 128 ? 0x1b : 0);
        i++;
    }
    return i;
}

// Galois field anti-logarithm calculation
uint8_t gf_alog(uint8_t x) {
    uint8_t y = 1;
    for (int i = 0; i < x; i++) {
        y = (y << 1) ^ (y >= 128 ? 0x1b : 0);
    }
    return y;
}

// Multiplicative inverse in Galois field
uint8_t gf_mulinv(uint8_t x) {
    if (x == 0) return 0;
    return gf_alog(255 - gf_log(x));
}

// S-box calculation using Galois field operations
uint8_t rj_sbox(uint8_t x) {
    uint8_t y = gf_mulinv(x);
    return y ^ ((y << 1) | (y >> 7)) ^ ((y << 2) | (y >> 6)) ^ 
           ((y << 3) | (y >> 5)) ^ ((y << 4) | (y >> 4)) ^ 0x63;
}

// X-time operation for MixColumns
uint8_t rj_xtime(uint8_t x) {
    return (x << 1) ^ (x >> 7) * 0x1b;
}

// SubBytes transformation
void aes_subBytes(uint8_t *buf) {
    for (int i = 0; i < 16; i++) {
        buf[i] = rj_sbox(buf[i]);
    }
}

// ShiftRows transformation
void aes_shiftRows(uint8_t *buf) {
    uint8_t temp;
    // Row 1: shift left by 1
    temp = buf[1];
    buf[1] = buf[5];
    buf[5] = buf[9];
    buf[9] = buf[13];
    buf[13] = temp;
    
    // Row 2: shift left by 2
    temp = buf[2];
    buf[2] = buf[10];
    buf[10] = temp;
    temp = buf[6];
    buf[6] = buf[14];
    buf[14] = temp;
    
    // Row 3: shift left by 3
    temp = buf[3];
    buf[3] = buf[15];
    buf[15] = buf[11];
    buf[11] = buf[7];
    buf[7] = temp;
}

// MixColumns transformation
void aes_mixColumns(uint8_t *buf) {
    for (int i = 0; i < 4; i++) {
        uint8_t s0 = buf[i*4 + 0];
        uint8_t s1 = buf[i*4 + 1];
        uint8_t s2 = buf[i*4 + 2];
        uint8_t s3 = buf[i*4 + 3];
        
        uint8_t h0 = s0 ^ s1 ^ s2 ^ s3;
        uint8_t h1 = s0 ^ s1;
        h1 = rj_xtime(h1) ^ h0;
        uint8_t h2 = s1 ^ s2;
        h2 = rj_xtime(h2) ^ h0;
        uint8_t h3 = s2 ^ s3;
        h3 = rj_xtime(h3) ^ h0;
        
        buf[i*4 + 0] = s0 ^ h1;
        buf[i*4 + 1] = s1 ^ h2;
        buf[i*4 + 2] = s2 ^ h3;
        buf[i*4 + 3] = s3 ^ h1;
    }
}

// AddRoundKey transformation
void aes_addRoundKey(uint8_t *buf, uint8_t *key) {
    for (int i = 0; i < 16; i++) {
        buf[i] ^= key[i];
    }
}

// Key expansion helper function
uint8_t F(uint8_t x) {
    return rj_sbox(x);
}

// Expand encryption key
void aes_expandEncKey(uint8_t *k, uint8_t *rc) {
    k[0] ^= F(k[29]) ^ (*rc);
    k[1] ^= F(k[30]);
    k[2] ^= F(k[31]);
    k[3] ^= F(k[28]);
    *rc = rj_xtime(*rc);
    
    for (int i = 4; i < 16; i++) {
        k[i] ^= k[i-4];
    }
    
    k[16] ^= F(k[12]);
    k[17] ^= F(k[13]);
    k[18] ^= F(k[14]);
    k[19] ^= F(k[15]);
    
    for (int i = 20; i < 32; i++) {
        k[i] ^= k[i-4];
    }
}

// Main AES-256 ECB encryption function
void aes256_encrypt_ecb(aes256_context *ctx, uint8_t k[32], uint8_t buf[16]) {
    // Copy key to context
    for (int i = 0; i < 32; i++) {
        ctx->key[i] = k[i];
    }
    
    // Initialize round constants
    uint8_t rc = 1;
    
    // Copy initial key to encryption key schedule
    for (int i = 0; i < 32; i++) {
        ctx->enckey[i] = ctx->key[i];
    }
    
    // Perform initial AddRoundKey
    aes_addRoundKey(buf, ctx->enckey);
    
    // Perform 13 rounds of encryption
    for (int round = 0; round < 13; round++) {
        // SubBytes
        aes_subBytes(buf);
        
        // ShiftRows
        aes_shiftRows(buf);
        
        // MixColumns (except for last round)
        aes_mixColumns(buf);
        
        // Expand key if needed
        if ((round & 1) == 0) {
            aes_expandEncKey(ctx->enckey, &rc);
        }
        
        // AddRoundKey
        aes_addRoundKey(buf, &ctx->enckey[((round + 1) % 2) * 16]);
    }
    
    // Final round (without MixColumns)
    aes_subBytes(buf);
    aes_shiftRows(buf);
    
    // Expand key for final round
    aes_expandEncKey(ctx->enckey, &rc);
    aes_addRoundKey(buf, &ctx->enckey[16]);
}
