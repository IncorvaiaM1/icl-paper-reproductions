
#include "spmv_crs.h"

void spmv(
    TYPE val[NNZ],
    int32_t cols[NNZ],
    int32_t rowDelimiters[N + 1],
    TYPE vec[N],
    TYPE out[N]) {
    
    // Loop over rows
    for (int i = 0; i < N; i++) {
        // Get the start and end indices for this row
        int tmp_begin = rowDelimiters[i];
        int tmp_end = rowDelimiters[i + 1];
        
        // Initialize sum for this row
        TYPE sum = 0.0;
        
        // Loop over non-zero elements in this row
        for (int j = tmp_begin; j < tmp_end; j++) {
            // Accumulate the product of matrix value and vector element
            sum += val[j] * vec[cols[j]];
        }
        
        // Store the result for this row
        out[i] = sum;
    }
}
