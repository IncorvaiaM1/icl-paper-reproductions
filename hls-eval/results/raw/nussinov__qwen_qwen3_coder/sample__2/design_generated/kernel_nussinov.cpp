
#include "nussinov.h"

// Scoring function to check if two bases are complementary
int score(char a, char b) {
    if ((a == 'A' && b == 'U') || (a == 'U' && b == 'A') ||
        (a == 'C' && b == 'G') || (a == 'G' && b == 'C') ||
        (a == 'A' && b == 'T') || (a == 'T' && b == 'A') ||
        (a == 'G' && b == 'C') || (a == 'C' && b == 'G')) {
        return 1;
    } else {
        return 0;
    }
}

void kernel_nussinov(char seq[60], int table[60][60]) {
    int i, j, k;
    int max_score;

    // Initialize the table
    for (i = 0; i < 60; i++) {
        for (j = 0; j < 60; j++) {
            table[i][j] = 0;
        }
    }

    // Fill the table using dynamic programming
    for (int length = 1; length < 60; length++) {
        for (i = 0; i < 60 - length; i++) {
            j = i + length;

            // Case 1: table[i+1][j]
            if (i + 1 < 60) {
                max_score = table[i + 1][j];
            } else {
                max_score = 0;
            }

            // Case 2: table[i][j-1]
            if (j - 1 >= 0) {
                int score2 = table[i][j - 1];
                if (score2 > max_score) {
                    max_score = score2;
                }
            }

            // Case 3: table[i+1][j-1] + w(i,j)
            if (i + 1 < 60 && j - 1 >= 0) {
                int score3 = table[i + 1][j - 1] + score(seq[i], seq[j]);
                if (score3 > max_score) {
                    max_score = score3;
                }
            }

            // Case 4: max over all k of (table[i][k] + table[k+1][j])
            int score4 = 0;
            for (k = i + 1; k < j; k++) {
                int temp = table[i][k] + table[k + 1][j];
                if (temp > score4) {
                    score4 = temp;
                }
            }
            if (score4 > max_score) {
                max_score = score4;
            }

            table[i][j] = max_score;
        }
    }
}
