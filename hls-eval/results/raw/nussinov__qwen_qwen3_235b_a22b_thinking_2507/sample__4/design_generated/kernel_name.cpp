
#include "nussinov.h"

void kernel_nussinov(char seq[60], int table[60][60]) {
    for (int L = 2; L <= 60; L++) {
        for (int i = 0; i <= 60 - L; i++) {
            int j = i + L - 1;
            
            int case1 = table[i+1][j];
            int case2 = table[i][j-1];
            int case3 = table[i+1][j-1];
            if ((seq[i] == 'A' && (seq[j] == 'U' || seq[j] == 'T')) ||
                (seq[i] == 'U' && seq[j] == 'A') ||
                (seq[i] == 'T' && seq[j] == 'A') ||
                (seq[i] == 'G' && seq[j] == 'C') ||
                (seq[i] == 'C' && seq[j] == 'G')) {
                case3 += 1;
            }
            
            int case4 = 0;
            for (int k = i+1; k < j; k++) {
                int split_val = table[i][k] + table[k+1][j];
                if (split_val > case4) {
                    case4 = split_val;
                }
            }
            
            int max_val = case1;
            if (case2 > max_val) max_val = case2;
            if (case3 > max_val) max_val = case3;
            if (case4 > max_val) max_val = case4;
            
            table[i][j] = max_val;
        }
    }
}
