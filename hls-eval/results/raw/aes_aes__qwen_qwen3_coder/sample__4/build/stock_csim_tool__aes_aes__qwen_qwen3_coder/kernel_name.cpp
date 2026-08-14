
#include "aes_aes.h"

// Helper functions for Galois Field operations
uint8_t gf_mul(uint8_t a, uint8_t b) {
    uint8_t p = 0;
    for (int i = 0; i < 8; i++) {
        if ((b & 1) == 1) {
            p ^= a;
        }
        if ((a & 0x80) == 0x80) {
            a = (a << 1) ^ 0x1b;
        } else {
            a <<= 1;
        }
        b >>= 1;
    }
    return p;
}

uint8_t gf_alog(uint8_t x) {
    uint8_t result = 1;
    for (int i = 0; i < x; i++) {
        if (result & 0x80) {
            result = (result << 1) ^ 0x1b;
        } else {
            result <<= 1;
        }
    }
    return result;
}

uint8_t gf_log(uint8_t x) {
    uint8_t result = 0;
    uint8_t temp = 1;
    while (temp != x && result < 255) {
        if (temp & 0x80) {
            temp = (temp << 1) ^ 0x1b;
        } else {
            temp <<= 1;
        }
        result++;
    }
    return result;
}

uint8_t gf_mulinv(uint8_t x) {
    if (x == 0) return 0;
    return gf_alog(255 - gf_log(x));
}

// S-box calculation
uint8_t rj_sbox(uint8_t x) {
    uint8_t y = gf_mulinv(x);
    uint8_t s = y ^ 
                ((y << 1) & 0xFF) ^ 
                ((y << 2) & 0xFF) ^ 
                ((y << 3) & 0xFF) ^ 
                ((y << 4) & 0xFF) ^ 
                0x63;
    return s;
}

// xtime operation
uint8_t rj_xtime(uint8_t x) {
    return (x << 1) ^ ((x >> 7) ? 0x1b : 0x00);
}

// AES sub-functions
void aes_subBytes(uint8_t *buf) {
    for (int i = 0; i < 16; i++) {
        buf[i] = rj_sbox(buf[i]);
    }
}

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

void aes_mixColumns(uint8_t *buf) {
    for (int i = 0; i < 4; i++) {
        uint8_t s0 = buf[4*i + 0];
        uint8_t s1 = buf[4*i + 1];
        uint8_t s2 = buf[4*i + 2];
        uint8_t s3 = buf[4*i + 3];
        
        uint8_t h0 = rj_xtime(s0) ^ s1 ^ s2 ^ rj_xtime(s3) ^ s3;
        uint8_t h1 = s0 ^ rj_xtime(s1) ^ s2 ^ s3 ^ rj_xtime(s3);
        uint8_t h2 = s0 ^ s1 ^ rj_xtime(s2) ^ s3 ^ rj_xtime(s3);
        uint8_t h3 = rj_xtime(s0) ^ s0 ^ s1 ^ s2 ^ s3;
        
        buf[4*i + 0] = h0;
        buf[4*i + 1] = h1;
        buf[4*i + 2] = h2;
        buf[4*i + 3] = h3;
    }
}

void aes_addRoundKey(uint8_t *buf, uint8_t *key) {
    for (int i = 0; i < 16; i++) {
        buf[i] ^= key[i];
    }
}

void aes_expandEncKey(uint8_t *k, uint8_t *rc) {
    uint8_t t0, t1, t2, t3;
    
    // Rotate and apply S-box
    t0 = rj_sbox(k[28]);
    t1 = rj_sbox(k[29]);
    t2 = rj_sbox(k[30]);
    t3 = rj_sbox(k[31]);
    
    // XOR with previous column and round constant
    k[0] ^= t0 ^ *rc;
    k[1] ^= t1;
    k[2] ^= t2;
    k[3] ^= t3;
    
    // Update round constant
    *rc = rj_xtime(*rc);
    
    // Calculate remaining columns
    for (int i = 4; i < 16; i += 4) {
        t0 = k[i];
        t1 = k[i+1];
        t2 = k[i+2];
        t3 = k[i+3];
        
        k[i] ^= k[i-4];
        k[i+1] ^= k[i-3];
        k[i+2] ^= k[i-2];
        k[i+3] ^= k[i-1];
    }
    
    // Apply S-box to second half
    for (int i = 16; i < 32; i += 4) {
        k[i] ^= rj_sbox(k[i-4]);
        k[i+1] ^= rj_sbox(k[i-3]);
        k[i+2] ^= rj_sbox(k[i-2]);
        k[i+3] ^= rj_sbox(k[i-1]);
    }
}

// Main AES-256 ECB encryption function
void aes256_encrypt_ecb(aes256_context *ctx, uint8_t k[32], uint8_t buf[16]) {
    // Copy key to context
    for (int i = 0; i < 32; i++) {
        ctx->key[i] = k[i];
    }
    
    // Initialize round keys
    uint8_t roundKey[16];
    for (int i = 0; i < 16; i++) {
        roundKey[i] = ctx->key[i];
    }
    
    // Initial round key addition
    aes_addRoundKey(buf, roundKey);
    
    // Set up round constant
    uint8_t rc = 1;
    uint8_t keyCopy[32];
    for (int i = 0; i < 32; i++) {
        keyCopy[i] = ctx->key[i];
    }
    
    // Main rounds
    for (int round = 1; round <= 13; round++) {
        // SubBytes
        aes_subBytes(buf);
        
        // ShiftRows
        aes_shiftRows(buf);
        
        // MixColumns (except last round)
        if (round < 13) {
            aes_mixColumns(buf);
        }
        
        // Generate next round key
        if ((round % 2) == 1) {
            aes_expandEncKey(keyCopy, &rc);
        }
        
        // Copy next round key
        for (int i = 0; i < 16; i++) {
            roundKey[i] = keyCopy[((round+1)/2)*16 + i % 16];
        }
        
        // Add round key
        aes_addRoundKey(buf, roundKey);
    }
}
