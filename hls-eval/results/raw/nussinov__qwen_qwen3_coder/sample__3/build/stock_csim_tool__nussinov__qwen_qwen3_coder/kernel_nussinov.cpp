
#include "nussinov.h"

void kernel_nussinov(char seq[60], int table[60][60]) {
    int i, j, k;
    int max_score, score1, score2, score3, score4;

    // Initialize the table
    for (i = 0; i < 60; i++) {
        for (j = 0; j < 60; j++) {
            table[i][j] = 0;
        }
    }

    // Fill the table using dynamic programming
    for (int gap = 1; gap < 60; gap++) {
        for (i = 0; i < 60 - gap; i++) {
            j = i + gap;

            // Case 1: table[i+1][j]
            score1 = table[i + 1][j];

            // Case 2: table[i][j-1]
            score2 = table[i][j - 1];

            // Case 3: table[i+1][j-1] + w(i,j)
            int w = 0;
            if ((seq[i] == 'A' && seq[j] == 'U') || (seq[i] == 'U' && seq[j] == 'A') ||
                (seq[i] == 'C' && seq[j] == 'G') || (seq[i] == 'G' && seq[j] == 'C')) {
                w = 1;
            }
            score3 = table[i + 1][j - 1] + w;

            // Case 4: max over k of (table[i][k] + table[k+1][j])
            score4 = 0;
            for (k = i + 1; k < j; k++) {
                int temp = table[i][k] + table[k + 1][j];
                if (temp > score4) {
                    score4 = temp;
                }
            }

            // Compute the maximum of the four scores
            max_score = score1;
            if (score2 > max_score) max_score = score2;
            if (score3 > max_score) max_score = score3;
            if (score4 > max_score) max_score = score4;

            table[i][j] = max_score;
        }
    }
}
