
#include "stencil_stencil3d.h"

void stencil3d(TYPE C[2], TYPE orig[SIZE], TYPE sol[SIZE]) {
    for (int k = 0; k < height_size; k++) {
        for (int j = 0; j < col_size; j++) {
            for (int i = 0; i < row_size; i++) {
                int idx = INDX(row_size, col_size, i, j, k);
                if (i == 0 || i == row_size-1 || 
                    j == 0 || j == col_size-1 || 
                    k == 0 || k == height_size-1) {
                    sol[idx] = orig[idx];
                } else {
                    int idx_k_plus = INDX(row_size, col_size, i, j, k+1);
                    int idx_k_minus = INDX(row_size, col_size, i, j, k-1);
                    int idx_j_plus = INDX(row_size, col_size, i, j+1, k);
                    int idx_j_minus = INDX(row_size, col_size, i, j-1, k);
                    int idx_i_plus = INDX(row_size, col_size, i+1, j, k);
                    int idx_i_minus = INDX(row_size, col_size, i-1, j, k);
                    
                    sol[idx] = C[0] * orig[idx] 
                             + C[1] * (orig[idx_k_plus] + orig[idx_k_minus] 
                                     + orig[idx_j_plus] + orig[idx_j_minus] 
                                     + orig[idx_i_plus] + orig[idx_i_minus]);
                }
            }
        }
    }
}
