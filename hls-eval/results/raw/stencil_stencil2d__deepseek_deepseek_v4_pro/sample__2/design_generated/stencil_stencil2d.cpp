
#include "stencil_stencil2d.h"

void stencil(TYPE orig[row_size * col_size], TYPE sol[row_size * col_size], TYPE filter[f_size]) {
    int i, j, k1, k2;
    TYPE temp, mul;

    for (i = 0; i < row_size - 2; i++) {
        for (j = 0; j < col_size - 2; j++) {
            temp = 0;
            for (k1 = 0; k1 < 3; k1++) {
                for (k2 = 0; k2 < 3; k2++) {
                    mul = filter[k1 * 3 + k2] * orig[(i + k1) * col_size + (j + k2)];
                    temp += mul;
                }
            }
            sol[i * col_size + j] = temp;
        }
    }
}
