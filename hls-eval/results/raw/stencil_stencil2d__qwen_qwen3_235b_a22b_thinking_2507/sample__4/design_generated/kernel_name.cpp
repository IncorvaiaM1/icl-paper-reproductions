
#include "stencil_stencil2d.h"

void stencil(TYPE orig[row_size * col_size], TYPE sol[row_size * col_size], TYPE filter[f_size]) {
    for (int i = 0; i < row_size; i++) {
        for (int j = 0; j < col_size; j++) {
            TYPE temp = 0;
            for (int k1 = 0; k1 < 3; k1++) {
                for (int k2 = 0; k2 < 3; k2++) {
                    int row = i + k1;
                    int col = j + k2;
                    TYPE val = 0;
                    if (row < row_size && col < col_size) {
                        val = orig[row * col_size + col];
                    }
                    TYPE mul = val * filter[k1 * 3 + k2];
                    temp += mul;
                }
            }
            sol[i * col_size + j] = temp;
        }
    }
}
