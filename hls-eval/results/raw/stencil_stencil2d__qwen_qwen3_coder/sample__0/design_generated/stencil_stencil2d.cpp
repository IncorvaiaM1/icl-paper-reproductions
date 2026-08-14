
#include "stencil_stencil2d.h"

void stencil(
    TYPE orig[row_size * col_size],
    TYPE sol[row_size * col_size],
    TYPE filter[f_size]) {

    // Iterate over the input array rows (0 to row_size-3)
    for (int i = 0; i < row_size - 2; i++) {
        // Iterate over the input array columns (0 to col_size-3)
        for (int j = 0; j < col_size - 2; j++) {
            // Temporary variable to accumulate the stencil result
            TYPE temp = 0;

            // Apply the 3x3 filter
            for (int k1 = 0; k1 < 3; k1++) {
                for (int k2 = 0; k2 < 3; k2++) {
                    // Calculate the index in the original array
                    int orig_index = (i + k1) * col_size + (j + k2);
                    // Calculate the index in the filter array
                    int filter_index = k1 * 3 + k2;
                    // Multiply and accumulate
                    temp += orig[orig_index] * filter[filter_index];
                }
            }

            // Store the result in the solution array
            sol[i * col_size + j] = temp;
        }
    }
}
