
#include "nussinov.h"

static inline bool is_pair(char a, char b) {
    if (a == 'A') {
        return (b == 'U' || b == 'T');
    }
    if (a == 'U' || a == 'T') {
        return (b == 'A');
    }
    if (a == 'G') {
        return (b == 'C');
    }
    if (a == 'C') {
        return (b == 'G');
    }
    return false;
}

void kernel_nussinov(char seq[60], int table[60][60]) {
    for (int L = 1; L < 60; L++) {
        for (int i = 0; i < 60 - L; i++) {
            int j = i + L;
            int case1 = table[i+1][j];
            int case2 = table[i][j-1];
            int score = is_pair(seq[i], seq[j]) ? 1 : 0;
            int case3 = table[i+1][j-1] + score;
            
            int max_val = case1;
            if (case2 > max_val) max_val = case2;
            if (case3 > max_val) max_val = case3;
            
            if (L >= 2) {
                int case4 = 0;
                for (int k = i+1; k < j; k++) {
                    int val = table[i][k] + table[k+1][j];
                    if (val > case4) case4 = val;
                }
                if (case4 > max_val) max_val = case4;
            }
            
            table[i][j] = max_val;
        }
    }
}
