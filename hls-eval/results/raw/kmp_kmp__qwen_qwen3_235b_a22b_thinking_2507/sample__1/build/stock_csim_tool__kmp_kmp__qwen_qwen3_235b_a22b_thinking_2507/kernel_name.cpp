
#include "kmp_kmp.h"

void CPF(char pattern[PATTERN_SIZE], int32_t kmpNext[PATTERN_SIZE]) {
    kmpNext[0] = -1;
    int i = -1;
    for (int j = 1; j < PATTERN_SIZE; j++) {
        while (i >= 0 && pattern[i+1] != pattern[j]) {
            i = kmpNext[i];
        }
        if (pattern[i+1] == pattern[j]) {
            i++;
        }
        kmpNext[j] = i;
    }
}

int kmp(char pattern[PATTERN_SIZE], char input[STRING_SIZE], int32_t kmpNext[PATTERN_SIZE], int32_t n_matches[1]) {
    CPF(pattern, kmpNext);
    
    int i = -1;
    int count = 0;
    
    for (int j = 0; j < STRING_SIZE; j++) {
        while (i >= 0 && pattern[i+1] != input[j]) {
            i = kmpNext[i];
        }
        if (pattern[i+1] == input[j]) {
            i++;
        }
        if (i == PATTERN_SIZE - 1) {
            count++;
            i = kmpNext[i];
        }
    }
    
    n_matches[0] = count;
    return count;
}
