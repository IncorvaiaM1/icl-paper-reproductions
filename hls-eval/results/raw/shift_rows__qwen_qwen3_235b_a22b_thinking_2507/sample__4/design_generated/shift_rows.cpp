
#include "shift_rows.h"

void ShiftRows(state_t *state) {
    for (int i = 0; i < 4; i++) {
        uint8_t temp[4];
        // Copy current row to temporary array
        for (int j = 0; j < 4; j++) {
            temp[j] = (*state)[i][j];
        }
        // Perform circular left shift by i positions
        for (int j = 0; j < 4; j++) {
            (*state)[i][j] = temp[(j + i) % 4];
        }
    }
}
