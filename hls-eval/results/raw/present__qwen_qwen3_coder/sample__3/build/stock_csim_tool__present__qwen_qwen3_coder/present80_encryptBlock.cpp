
#include "present.h"

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

void addRoundKey(block_t *block, round_key_t *roundKey) {
    for (int i = 0; i < PRESENT_BLOCK_SIZE_BYTES; i++) {
        (*block)[i] ^= (*roundKey)[i];
    }
}

void generateRoundKeys80(present_key_t *suppliedKey, keys_t *keys) {
    present_key_t key;
    copyKey(suppliedKey, &key);

    for (int round = 0; round < ROUNDS; round++) {
        // Copy the first 8 bytes (64 bits) of the key to the round key
        for (int i = 0; i < ROUND_KEY_SIZE_BYTES; i++) {
            (*keys)[round][i] = key[i];
        }

        // Rotate the key to the left by 61 bits
        present_key_t tempKey;
        for (int i = 0; i < PRESENT_80_KEY_SIZE_BYTES; i++) {
            tempKey[i] = key[(i + 7) % PRESENT_80_KEY_SIZE_BYTES];
        }

        // Move the remaining 16 bits (bits 64-79) to the front
        for (int i = 0; i < PRESENT_80_KEY_SIZE_BYTES; i++) {
            key[i] = (tempKey[i] << 5) | (tempKey[(i + 1) % PRESENT_80_KEY_SIZE_BYTES] >> 3);
        }

        // Apply S-box to the 4-bit nibble in bits 76-79 (last nibble of the key)
        unsigned char nibble = key[PRESENT_80_KEY_SIZE_BYTES - 1] >> 4;
        key[PRESENT_80_KEY_SIZE_BYTES - 1] = (sBox[nibble] << 4) | (key[PRESENT_80_KEY_SIZE_BYTES - 1] & 0x0F);

        // XOR with round counter (using bits 15-79 of the key)
        // The round constant is added to bits 15-20 (the third byte from the left)
        key[1] ^= (round + 1) << 7;
        key[0] ^= (round + 1) >> 1;
    }
}

void sBoxLayer(block_t *block) {
    for (int i = 0; i < PRESENT_BLOCK_SIZE_BYTES; i++) {
        unsigned char byte = (*block)[i];
        unsigned char low_nibble = byte & 0x0F;
        unsigned char high_nibble = (byte >> 4) & 0x0F;
        (*block)[i] = (sBox[high_nibble] << 4) | sBox[low_nibble];
    }
}

void pLayer(block_t *block) {
    block_t tempBlock = {0};
    for (int i = 0; i < 64; i++) {
        int bitPos = (16 * i) % 63;
        if (i == 63) {
            bitPos = 63;
        }

        int srcByte = i / 8;
        int srcBit = i % 8;
        unsigned char bit = ((*block)[srcByte] >> srcBit) & 1;

        int dstByte = bitPos / 8;
        int dstBit = bitPos % 8;
        tempBlock[dstByte] |= (bit << dstBit);
    }
    copyBlock(&tempBlock, block);
}

void present80_encryptBlock(block_t *block, present_key_t *key) {
    keys_t roundKeys;
    generateRoundKeys80(key, &roundKeys);

    addRoundKey(block, (round_key_t *)&roundKeys[0]);

    for (int round = 1; round < ROUNDS; round++) {
        sBoxLayer(block);
        pLayer(block);
        addRoundKey(block, (round_key_t *)&roundKeys[round]);
    }

    sBoxLayer(block);
    pLayer(block);
    addRoundKey(block, (round_key_t *)&roundKeys[ROUNDS]);
}
