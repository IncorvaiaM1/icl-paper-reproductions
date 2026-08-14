
#include "spmv_crs.h"

void spmv(
    TYPE val[NNZ],
    int32_t cols[NNZ],
    int32_t rowDelimiters[N + 1],
    TYPE vec[N],
    TYPE out[N]) {
    
    // Outer loop: iterate over rows
    for (int i = 0; i < N; i++) {
        // Get the start and end indices for the current row
        int tmp_begin = rowDelimiters[i];
        int tmp_end = rowDelimiters[i + 1];
        
        // Initialize sum for the current row
        TYPE sum = 0.0;
        
        // Inner loop: iterate over non-zero elements of the current row
        for (int j = tmp_begin; j < tmp_end; j++) {
            // Compute product of matrix element and corresponding vector element
            TYPE product = val[j] * vec[cols[j]];
            // Accumulate the product into the sum
            sum += product;
        }
        
        // Store the computed sum in the output vector
        out[i] = sum;
    }
}
