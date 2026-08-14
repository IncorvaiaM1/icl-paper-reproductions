
#include "present.h"

static void generateRoundKeys80(present_key_t *suppliedKey, keys_t *keys) {
    present_key_t current_key;
    for (int i = 0; i < PRESENT_80_KEY_SIZE_BYTES; i++) {
        current_key[i] = (*suppliedKey)[i];
    }

    for (int round = 0; round < ROUNDS; round++) {
        for (int j = 0; j < ROUND_KEY_SIZE_BYTES; j++) {
            (*keys)[round][j] = current_key[j];
        }

        unsigned char temp = (current_key[0] >> 4) | (current_key[1] << 4);
        current_key[1] = (current_key[1] >> 4) | (current_key[2] << 4);
        current_key[2] = (current_key[2] >> 4) | (current_key[3] << 4);
        current_key[3] = (current_key[3] >> 4) | (current_key[4] << 4);
        current_key[4] = (current_key[4] >> 4) | (current_key[5] << 4);
        current_key[5] = (current_key[5] >> 4) | (current_key[6] << 4);
        current_key[6] = (current_key[6] >> 4) | (current_key[7] << 4);
        current_key[7] = (current_key[7] >> 4) | (current_key[8] << 4);
        current_key[8] = (current_key[8] >> 4) | (current_key[9] << 4);
        current_key[9] = (current_key[9] >> 4) | (temp << 4);
        current_key[0] = temp;

        current_key[0] = (sBox[current_key[0] >> 4] << 4) | (current_key[0] & 0x0F);

        current_key[9] = current_key[9] ^ round;
        current_key[9] = current_key[9] ^ (round << 4);
    }
}

void present80_encryptBlock(block_t *block, present_key_t *key) {
    keys_t roundKeys;
    generateRoundKeys80(key, &roundKeys);

    block_t currentBlock;
    for (int i = 0; i < PRESENT_BLOCK_SIZE_BYTES; i++) {
        currentBlock[i] = (*block)[i];
    }

    for (int round = 0; round < 31; round++) {
        addRoundKey(&currentBlock, &roundKeys[round]);

        block_t substitutedBlock;
        for (int i = 0; i < PRESENT_BLOCK_SIZE_BYTES; i++) {
            substitutedBlock[i] = (sBox[currentBlock[i] >> 4] << 4) | sBox[currentBlock[i] & 0x0F];
        }

        pLayer(&substitutedBlock);

        for (int i = 0; i < PRESENT_BLOCK_SIZE_BYTES; i++) {
            currentBlock[i] = substitutedBlock[i];
        }
    }

    addRoundKey(&currentBlock, &roundKeys[31]);

    for (int i = 0; i < PRESENT_BLOCK_SIZE_BYTES; i++) {
        (*block)[i] = currentBlock[i];
    }
}
