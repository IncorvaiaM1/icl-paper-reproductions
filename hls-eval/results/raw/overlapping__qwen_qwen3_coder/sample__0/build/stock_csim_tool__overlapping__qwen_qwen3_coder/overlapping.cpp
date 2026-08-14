
#include "overlapping.h"

void Overlapping(double *result, int epsilon[N]) {
    unsigned int nu[6] = {0, 0, 0, 0, 0, 0};
    const double pi[6] = {0.05, 0.15, 0.25, 0.30, 0.20, 0.05};
    const char sequence[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};

    int window_size = 1024;
    int num_windows = N - window_size + 1;

    // Iterate over each window
    for (int i = 0; i < num_windows; i++) {
        int count = 0;

        // Count matches of the pattern within the current window
        for (int j = 0; j <= window_size - 9; j++) {
            int match = 1;
            for (int k = 0; k < 9; k++) {
                if (epsilon[i + j + k] != sequence[k]) {
                    match = 0;
                    break;
                }
            }
            if (match) {
                count++;
            }
        }

        // Update nu based on count
        if (count <= 4) {
            nu[count]++;
        } else {
            nu[5]++;
        }
    }

    // Compute chi-square statistic
    *result = 0.0;
    for (int i = 0; i < 6; i++) {
        double expected = pi[i] * num_windows;
        if (expected > 0) {
            double diff = (double)nu[i] - expected;
            *result += (diff * diff) / expected;
        }
    }
}
