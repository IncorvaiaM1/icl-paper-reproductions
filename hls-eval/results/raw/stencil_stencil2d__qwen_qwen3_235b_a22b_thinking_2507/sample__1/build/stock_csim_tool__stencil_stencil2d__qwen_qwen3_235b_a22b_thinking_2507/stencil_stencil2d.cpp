
#include "stencil_stencil2d.h"

void stencil(TYPE orig[row_size * col_size], TYPE sol[row_size * col_size], TYPE filter[f_size]) {
    for (int i = 0; i < row_size; i++) {
        for (int j = 0; j < col_size; j++) {
            TYPE temp = 0;
            for (int k1 = 0; k1 < 3; k1++) {
                for (int k2 = 0; k2 < 3; k2++) {
                    int row_index = i + k1;
                    int col_index = j + k2;
                    if (row_index < row_size && col_index < col_size) {
                        TYPE mul = orig[row_index * col_size + col_index] * filter[k1 * 3 + k2];
                        temp += mul;
                    }
                }
            }
            sol[i * col_size + j] = temp;
        }
    }
}
