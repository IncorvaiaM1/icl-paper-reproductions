
#include "present.h"

void generateRoundKeys80(present_key_t *suppliedKey, keys_t *keys) {
    present_key_t key;
    copyKey(suppliedKey, &key);

    // Generate round keys
    for (int i = 0; i < ROUNDS; i++) {
        // Copy leftmost 64 bits as round key
        for (int j = 0; j < ROUND_KEY_SIZE_BYTES; j++) {
            (*keys)[i][j] = key[j];
        }

        // Key schedule
        // 1. Left rotate by 61 bits (19 bytes and 5 bits)
        unsigned char temp[PRESENT_80_KEY_SIZE_BYTES];
        for (int j = 0; j < PRESENT_80_KEY_SIZE_BYTES; j++) {
            temp[j] = (key[(j + 19) % PRESENT_80_KEY_SIZE_BYTES] >> 5) |
                      (key[(j + 20) % PRESENT_80_KEY_SIZE_BYTES] << 3);
        }

        // 2. S-box substitution on leftmost nibble
        temp[0] = (sBox[temp[0] >> 4] << 4) | (temp[0] & 0x0F);

        // 3. XOR with round counter
        temp[PRESENT_80_KEY_SIZE_BYTES - 1] ^= (i + 1) << 4;

        // Update key for next round
        for (int j = 0; j < PRESENT_80_KEY_SIZE_BYTES; j++) {
            key[j] = temp[j];
        }
    }
}

void addRoundKey(block_t *block, round_key_t *roundKey) {
    for (int i = 0; i < PRESENT_BLOCK_SIZE_BYTES; i++) {
        (*block)[i] ^= (*roundKey)[i];
    }
}

void pLayer(block_t *block) {
    block_t temp;
    for (int i = 0; i < PRESENT_BLOCK_SIZE_BYTES; i++) {
        temp[i] = 0;
    }

    for (int i = 0; i < 64; i++) {
        int src_bit = i;
        int dst_bit = (i * 16) % 63;
        if (i == 63) dst_bit = 63;

        int src_byte = src_bit / 8;
        int src_bit_in_byte = src_bit % 8;
        int dst_byte = dst_bit / 8;
        int dst_bit_in_byte = dst_bit % 8;

        if ((*block)[src_byte] & (1 << src_bit_in_byte)) {
            temp[dst_byte] |= (1 << dst_bit_in_byte);
        }
    }

    copyBlock(&temp, block);
}

void copyKey(present_key_t *from, present_key_t *to) {
    for (int i = 0; i < PRESENT_80_KEY_SIZE_BYTES; i++) {
        (*to)[i] = (*from)[i];
    }
}

void copyBlock(block_t *from, block_t *to) {
    for (int i = 0; i < PRESENT_BLOCK_SIZE_BYTES; i++) {
        (*to)[i] = (*from)[i];
    }
}

void present80_encryptBlock(block_t *block, present_key_t *key) {
    keys_t roundKeys;
    generateRoundKeys80(key, &roundKeys);

    for (int round = 0; round < ROUNDS; round++) {
        // Add round key
        addRoundKey(block, &roundKeys[round]);

        // S-box substitution
        for (int i = 0; i < PRESENT_BLOCK_SIZE_BYTES; i++) {
            (*block)[i] = (sBox[(*block)[i] >> 4] << 4) | sBox[(*block)[i] & 0x0F];
        }

        // Permutation layer (except last round)
        if (round != ROUNDS - 1) {
            pLayer(block);
        }
    }

    // Final round key addition
    addRoundKey(block, &roundKeys[ROUNDS - 1]);
}
