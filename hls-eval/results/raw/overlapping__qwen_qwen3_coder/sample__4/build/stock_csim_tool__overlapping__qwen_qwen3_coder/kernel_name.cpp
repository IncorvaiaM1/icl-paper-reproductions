
#include "overlapping.h"

void Overlapping(double *result, int epsilon[N]) {
    const char sequence[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    unsigned int nu[6] = {0, 0, 0, 0, 0, 0};
    const double pi[6] = {
        0.052344,
        0.166038,
        0.235315,
        0.290781,
        0.195680,
        0.059842
    };

    int window_size = 1024;
    int pattern_length = 9;
    int num_windows = N - window_size + 1;

    // Iterate through each sliding window
    for (int i = 0; i < num_windows; i++) {
        int count = 0;

        // Check for pattern matches within the current window
        for (int j = 0; j < window_size - pattern_length + 1; j++) {
            bool match = true;
            for (int k = 0; k < pattern_length; k++) {
                if (epsilon[i + j + k] != sequence[k]) {
                    match = false;
                    break;
                }
            }
            if (match) {
                count++;
            }
        }

        // Update nu array based on count
        if (count >= 5) {
            nu[5]++;
        } else {
            nu[count]++;
        }
    }

    // Compute chi-square statistic
    double chi_square = 0.0;
    for (int i = 0; i < 6; i++) {
        double expected = pi[i] * num_windows;
        double diff = (double)nu[i] - expected;
        chi_square += (diff * diff) / expected;
    }

    *result = chi_square;
}
