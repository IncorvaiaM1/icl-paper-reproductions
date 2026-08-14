
#include "block.h"

void BlockFrequency(double *result, int epsilon[M * N]) {
    double sum = 0.0;
    
    for (int i = 0; i < M; i++) {
        int blockSum = 0;
        
        // Compute sum of current row
        for (int j = 0; j < N; j++) {
            blockSum += epsilon[i * N + j];
        }
        
        // Compute proportion and deviation
        double pi = (double)blockSum / N;
        double v = pi - 0.5;
        
        // Add squared deviation to sum
        sum += v * v;
    }
    
    // Store final result
    *result = sum;
}
