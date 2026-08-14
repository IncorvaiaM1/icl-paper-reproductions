
#include "nussinov.h"

// Scoring function for complementary base pairs
int score(int i, int j, char seq[60]) {
    char a = seq[i];
    char b = seq[j];
    if ((a == 'A' && b == 'U') || (a == 'U' && b == 'A')) {
        return 1;
    } else if ((a == 'C' && b == 'G') || (a == 'G' && b == 'C')) {
        return 1;
    } else {
        return 0;
    }
}

void kernel_nussinov(char seq[60], int table[60][60]) {
    int i, j, k;
    int n = 60;

    // Initialize the table to zero
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            table[i][j] = 0;
        }
    }

    // Fill the table using dynamic programming
    for (int l = 1; l < n; l++) {  // l is the length of the subsequence
        for (i = 0; i < n - l; i++) {
            j = i + l;
            // Case 1: table[i+1][j]
            int t1 = table[i + 1][j];
            // Case 2: table[i][j-1]
            int t2 = table[i][j - 1];
            // Case 3: table[i+1][j-1] + w(i,j)
            int t3 = table[i + 1][j - 1] + score(i, j, seq);
            // Case 4: max over k of (table[i][k] + table[k+1][j])
            int t4 = 0;
            for (k = i + 1; k < j; k++) {
                int temp = table[i][k] + table[k + 1][j];
                if (temp > t4) {
                    t4 = temp;
                }
            }
            // Take the maximum of all four cases
            int max_val = t1;
            if (t2 > max_val) max_val = t2;
            if (t3 > max_val) max_val = t3;
            if (t4 > max_val) max_val = t4;
            table[i][j] = max_val;
        }
    }
}
