
#include "sub.h"

void SubBytes(state_t *state) {
    state_t &s = *state;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            s[i][j] = getSBoxValue(s[i][j]);
        }
    }
}
