
#include "add_round_key.h"

void AddRoundKey(unsigned char round, unsigned char (*state)[4][4], unsigned char const *RoundKey) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            int index = round * (Nb * 4) + i * Nb + j;
            (*state)[i][j] ^= RoundKey[index];
        }
    }
}
