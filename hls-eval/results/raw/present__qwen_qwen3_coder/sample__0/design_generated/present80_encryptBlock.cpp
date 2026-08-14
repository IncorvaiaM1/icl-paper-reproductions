
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
    for (int i = 0; i < ROUND_KEY_SIZE_BYTES; i++) {
        (*block)[i] ^= (*roundKey)[i];
    }
}

void pLayer(block_t *block) {
    block_t tempBlock = {0};
    for (int i = 0; i < 64; i++) {
        int rowNum = i / 8;
        int colNum = i % 8;
        unsigned char bit = ((*block)[rowNum] >> colNum) & 0x1;

        int targetBitPos = (i * 16) % 63;
        if (i == 63) targetBitPos = 63;

        int targetRow = targetBitPos / 8;
        int targetCol = targetBitPos % 8;

        tempBlock[targetRow] |= (bit << targetCol);
    }
    copyBlock(&tempBlock, block);
}

void generateRoundKeys80(present_key_t *suppliedKey, keys_t *keys) {
    present_key_t currentKey;
    copyKey(suppliedKey, &currentKey);

    for (int round = 0; round < ROUNDS + 1; round++) {
        // Copy the first 8 bytes (64 bits) of the key to the round key
        for (int i = 0; i < 8; i++) {
            (*keys)[round][i] = currentKey[i];
        }

        if (round < ROUNDS) {
            // Key schedule: rotate left by 61 bits
            present_key_t tempKey = {0};
            for (int i = 0; i < 80; i++) {
                int srcByte = i / 8;
                int srcBit = i % 8;
                unsigned char bit = (currentKey[srcByte] >> srcBit) & 0x1;

                int destPos = (i + 61) % 80;
                int destByte = destPos / 8;
                int destBit = destPos % 8;

                tempKey[destByte] |= (bit << destBit);
            }

            // Apply S-box to the 4 leftmost bits (bits 76-79)
            unsigned char left4Bits = tempKey[9] >> 4;
            left4Bits = sBox[left4Bits];
            tempKey[9] = (tempKey[9] & 0x0F) | (left4Bits << 4);

            // XOR with round counter (bits 15-19 of the key)
            tempKey[7] ^= (round + 1) << 7;
            tempKey[8] ^= (round + 1) >> 1;

            copyKey(&tempKey, &currentKey);
        }
    }
}

void present80_encryptBlock(block_t *block, present_key_t *key) {
    keys_t roundKeys;
    generateRoundKeys80(key, &roundKeys);

    // Initial key addition
    addRoundKey(block, (round_key_t *)&roundKeys[0]);

    // 31 rounds of full transformation
    for (int round = 1; round < ROUNDS; round++) {
        // Substitution layer
        for (int i = 0; i < 8; i++) {
            unsigned char byte = (*block)[i];
            unsigned char left_nibble = (byte >> 4) & 0xF;
            unsigned char right_nibble = byte & 0xF;
            (*block)[i] = (sBox[left_nibble] << 4) | sBox[right_nibble];
        }

        // Permutation layer
        pLayer(block);

        // Key addition
        addRoundKey(block, (round_key_t *)&roundKeys[round]);
    }

    // Substitution layer for the 32nd round
    for (int i = 0; i < 8; i++) {
        unsigned char byte = (*block)[i];
        unsigned char left_nibble = (byte >> 4) & 0xF;
        unsigned char right_nibble = byte & 0xF;
        (*block)[i] = (sBox[left_nibble] << 4) | sBox[right_nibble];
    }

    // Permutation layer for the 32nd round
    pLayer(block);

    // Final key addition
    addRoundKey(block, (round_key_t *)&roundKeys[32]);
}
