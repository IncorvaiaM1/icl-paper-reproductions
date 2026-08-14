
#include "add_round_key.h"

void AddRoundKey(uint8_t round, state_t *state, const uint8_t *RoundKey) {
    for (uint8_t i = 0; i < 4; ++i) {
        for (uint8_t j = 0; j < 4; ++j) {
            (*state)[i][j] ^= RoundKey[(round * Nb * 4) + (i * Nb) + j];
        }
    }
}
