
#include "stencil_stencil3d.h"

void stencil3d(TYPE C[2], TYPE orig[SIZE], TYPE sol[SIZE]) {
    // Handle boundary conditions: fill boundaries with original values
    for (int i = 0; i < row_size; i++) {
        for (int j = 0; j < col_size; j++) {
            for (int k = 0; k < height_size; k++) {
                int idx = INDX(row_size, col_size, i, j, k);
                if (i == 0 || i == row_size - 1 ||
                    j == 0 || j == col_size - 1 ||
                    k == 0 || k == height_size - 1) {
                    sol[idx] = orig[idx];
                }
            }
        }
    }

    // Apply stencil computation to interior points
    for (int i = 1; i < row_size - 1; i++) {
        for (int j = 1; j < col_size - 1; j++) {
            for (int k = 1; k < height_size - 1; k++) {
                int idx = INDX(row_size, col_size, i, j, k);
                
                // Compute the weighted sum using the stencil formula
                sol[idx] = C[0] * orig[idx] +
                           C[1] * (
                               orig[INDX(row_size, col_size, i, j, k+1)] +  // orig[i][j][k+1]
                               orig[INDX(row_size, col_size, i, j, k-1)] +  // orig[i][j][k-1]
                               orig[INDX(row_size, col_size, i, j+1, k)] +  // orig[i][j+1][k]
                               orig[INDX(row_size, col_size, i, j-1, k)] +  // orig[i][j-1][k]
                               orig[INDX(row_size, col_size, i+1, j, k)] +  // orig[i+1][j][k]
                               orig[INDX(row_size, col_size, i-1, j, k)]    // orig[i-1][j][k]
                           );
            }
        }
    }
}
