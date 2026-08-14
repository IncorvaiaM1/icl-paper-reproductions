
#include "stencil_stencil2d.h"

void stencil(
    TYPE orig[row_size * col_size],
    TYPE sol[row_size * col_size],
    TYPE filter[f_size]) {

    // HLS pragmas for performance optimization
    #pragma HLS INTERFACE m_axi port=orig bundle=gmem0
    #pragma HLS INTERFACE m_axi port=sol bundle=gmem1
    #pragma HLS INTERFACE m_axi port=filter bundle=gmem2
    #pragma HLS INTERFACE s_axilite port=return bundle=control

    // Iterate over the input array
    ROW_LOOP: for (int i = 0; i < row_size - 2; i++) {
        #pragma HLS PIPELINE II=1
        COL_LOOP: for (int j = 0; j < col_size - 2; j++) {
            #pragma HLS UNROLL FACTOR=1
            
            // Temporary variable to accumulate results
            TYPE temp = 0;
            
            // Apply the 3x3 filter
            FILTER_LOOP_1: for (int k1 = 0; k1 < 3; k1++) {
                FILTER_LOOP_2: for (int k2 = 0; k2 < 3; k2++) {
                    #pragma HLS UNROLL
                    
                    // Calculate indices
                    int orig_idx = (i + k1) * col_size + (j + k2);
                    int filter_idx = k1 * 3 + k2;
                    
                    // Multiply and accumulate
                    TYPE mul = orig[orig_idx] * filter[filter_idx];
                    temp += mul;
                }
            }
            
            // Store the result
            int sol_idx = (i + 1) * col_size + (j + 1);
            sol[sol_idx] = temp;
        }
    }
}
