
#include "overlapping.h"

void Overlapping(double *result, int epsilon[N]) {
    const int M = 1032;  // Block size for overlapping template test (NIST standard for m=9)
    const int num_blocks = N / M;  // 1056768 / 1032 = 1024 blocks
    
    // Initialize count array for occurrences (0-4 matches and 5+ matches)
    unsigned int nu[6] = {0};
    
    // Predefined probabilities from NIST SP 800-22 for m=9, M=1032
    double pi[6] = {0.035170, 0.105510, 0.172183, 0.179757, 0.123671, 0.383709};
    
    // Process each block
    for (int block = 0; block < num_blocks; block++) {
        int count = 0;
        int start_index = block * M;
        
        // Slide window through the block (overlapping by 1 bit)
        for (int j = 0; j <= M - 9; j++) {
            bool is_match = true;
            
            // Check 9 consecutive bits for all ones
            for (int k = 0; k < 9; k++) {
                if (epsilon[start_index + j + k] != 1) {
                    is_match = false;
                    break;
                }
            }
            
            if (is_match) {
                count++;
            }
        }
        
        // Update count array (0-4 matches go to respective bins, 5+ go to bin 5)
        if (count < 5) {
            nu[count]++;
        } else {
            nu[5]++;
        }
    }
    
    // Calculate chi-square statistic
    double chi_square = 0.0;
    for (int k = 0; k < 6; k++) {
        double expected = num_blocks * pi[k];
        double diff = nu[k] - expected;
        chi_square += (diff * diff) / expected;
    }
    
    *result = chi_square;
}
