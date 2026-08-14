
#include "kmp_kmp.h"

void CPF(char pattern[PATTERN_SIZE], int32_t kmpNext[PATTERN_SIZE]) {
    int32_t k = 0;
    int32_t q = 1;
    kmpNext[0] = 0;

    CPF_LOOP: for (q = 1; q < PATTERN_SIZE; q++) {
        #pragma HLS LOOP_TRIPCOUNT min = 3 max = 3
        while (k > 0 && pattern[q] != pattern[k]) {
            #pragma HLS LOOP_TRIPCOUNT min = 1 max = 1
            k = kmpNext[k - 1];
        }
        if (pattern[q] == pattern[k]) {
            k++;
        }
        kmpNext[q] = k;
    }
}

int kmp(char pattern[PATTERN_SIZE], char input[STRING_SIZE], int32_t kmpNext[PATTERN_SIZE], int32_t n_matches[1]) {
    // Initialize n_matches to 0
    n_matches[0] = 0;

    // Preprocess the pattern to build kmpNext table
    CPF(pattern, kmpNext);

    // Search for pattern in input string
    int32_t q = 0;  // Number of characters matched

    SEARCH_LOOP: for (int32_t i = 0; i < STRING_SIZE; i++) {
        #pragma HLS LOOP_TRIPCOUNT min = 32411 max = 32411
        while (q > 0 && pattern[q] != input[i]) {
            #pragma HLS LOOP_TRIPCOUNT min = 1 max = 1
            q = kmpNext[q - 1];
        }
        if (pattern[q] == input[i]) {
            q++;
        }
        if (q >= PATTERN_SIZE) {
            // Pattern found
            n_matches[0]++;
            q = kmpNext[q - 1];
        }
    }

    return n_matches[0];
}
