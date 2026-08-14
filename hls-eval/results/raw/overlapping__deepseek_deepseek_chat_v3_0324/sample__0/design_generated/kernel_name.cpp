
#include "overlapping.h"

void Overlapping(double *result, int epsilon[N]) {
    // Initialize pattern to match (nine consecutive ones)
    const char sequence[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    
    // Predefined weights for chi-square calculation
    const double pi[6] = {0.364091, 0.185659, 0.139381, 0.100571, 0.0704323, 0.139865};
    
    // Initialize count array
    unsigned int nu[6] = {0};
    
    // Slide window through the dataset
    for (int i = 0; i <= N - 9; i++) {
        int match_count = 0;
        
        // Check for pattern matches in current window
        for (int j = 0; j < 9; j++) {
            if (epsilon[i + j] == sequence[j]) {
                match_count++;
            }
        }
        
        // Update count array based on number of matches
        if (match_count <= 4) {
            nu[match_count]++;
        } else {
            nu[5]++; // For 5 or more matches
        }
    }
    
    // Calculate chi-square statistic
    double chi_square = 0.0;
    for (int k = 0; k < 6; k++) {
        double expected = (N - 8) * pi[k]; // N-8 because we have N-8 windows
        double observed = nu[k];
        chi_square += ((observed - expected) * (observed - expected)) / expected;
    }
    
    // Store result
    *result = chi_square;
}
