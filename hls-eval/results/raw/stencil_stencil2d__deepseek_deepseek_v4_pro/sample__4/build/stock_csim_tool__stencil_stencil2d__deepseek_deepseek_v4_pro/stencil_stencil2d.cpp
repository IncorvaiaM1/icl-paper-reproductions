
#include "stencil_stencil2d.h"

void stencil(
    TYPE orig[row_size * col_size],
    TYPE sol[row_size * col_size],
    TYPE filter[f_size])
{
    // Compute the 3x3 stencil for all valid interior points.
    // The output array sol is assumed to be initially zero (as done by the testbench),
    // and border elements that cannot be computed remain zero.
    for (int i = 0; i < row_size - 2; i++) {
        for (int j = 0; j < col_size - 2; j++) {
            TYPE temp = 0;
            // Apply the 3x3 filter
            for (int k1 = 0; k1 < 3; k1++) {
                for (int k2 = 0; k2 < 3; k2++) {
                    TYPE mul = orig[(i + k1) * col_size + (j + k2)] * filter[k1 * 3 + k2];
                    temp += mul;
                }
            }
            sol[i * col_size + j] = temp;
        }
    }
}
