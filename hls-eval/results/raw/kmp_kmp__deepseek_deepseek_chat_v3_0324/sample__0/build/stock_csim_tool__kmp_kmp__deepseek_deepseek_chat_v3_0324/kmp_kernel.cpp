
#include "kmp_kmp.h"

void CPF(char pattern[PATTERN_SIZE], int32_t kmpNext[PATTERN_SIZE]) {
    int k, q;
    kmpNext[0] = 0;
    k = 0;

    for (q = 1; q < PATTERN_SIZE; q++) {
        while (k > 0 && pattern[q] != pattern[k]) {
            k = kmpNext[k-1];
        }
        if (pattern[q] == pattern[k]) {
            k++;
        }
        kmpNext[q] = k;
    }
}

int kmp(char pattern[PATTERN_SIZE], char input[STRING_SIZE], int32_t kmpNext[PATTERN_SIZE], int32_t n_matches[1]) {
    int i, q;
    int matches = 0;

    // Preprocess the pattern to build kmpNext table
    CPF(pattern, kmpNext);

    // Search for pattern in input string
    q = 0;
    for (i = 0; i < STRING_SIZE && input[i] != '\0'; i++) {
        while (q > 0 && pattern[q] != input[i]) {
            q = kmpNext[q-1];
        }
        if (pattern[q] == input[i]) {
            q++;
        }
        if (q == PATTERN_SIZE) {
            matches++;
            q = kmpNext[q-1];
        }
    }

    n_matches[0] = matches;
    return matches;
}
