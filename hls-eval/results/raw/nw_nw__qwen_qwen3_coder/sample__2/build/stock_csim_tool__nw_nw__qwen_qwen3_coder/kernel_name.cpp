
#include "nw_nw.h"

#define MATCH_SCORE 1
#define MISMATCH_SCORE -1
#define GAP_SCORE -1

void needwun(
    char SEQA[ALEN],
    char SEQB[BLEN],
    char alignedA[ALEN + BLEN],
    char alignedB[ALEN + BLEN],
    int M[(ALEN + 1) * (BLEN + 1)],
    char ptr[(ALEN + 1) * (BLEN + 1)])
{
    // Initialize first row
    for (int j = 0; j <= BLEN; j++) {
        M[0 * (BLEN + 1) + j] = j * GAP_SCORE;
        ptr[0 * (BLEN + 1) + j] = 'L'; // Left
    }

    // Initialize first column
    for (int i = 0; i <= ALEN; i++) {
        M[i * (BLEN + 1) + 0] = i * GAP_SCORE;
        ptr[i * (BLEN + 1) + 0] = 'U'; // Up
    }

    // Fill the matrix
    for (int i = 1; i <= ALEN; i++) {
        for (int j = 1; j <= BLEN; j++) {
            int diag_score = M[(i-1) * (BLEN + 1) + (j-1)] + 
                ((SEQA[i-1] == SEQB[j-1]) ? MATCH_SCORE : MISMATCH_SCORE);
            int up_score = M[(i-1) * (BLEN + 1) + j] + GAP_SCORE;
            int left_score = M[i * (BLEN + 1) + (j-1)] + GAP_SCORE;

            int max_score = diag_score;
            char direction = 'D'; // Diagonal

            if (up_score > max_score) {
                max_score = up_score;
                direction = 'U'; // Up
            }
            if (left_score > max_score) {
                max_score = left_score;
                direction = 'L'; // Left
            }

            M[i * (BLEN + 1) + j] = max_score;
            ptr[i * (BLEN + 1) + j] = direction;
        }
    }

    // Traceback
    int i = ALEN;
    int j = BLEN;
    int align_idx = ALEN + BLEN - 1;

    while (i > 0 || j > 0) {
        if (ptr[i * (BLEN + 1) + j] == 'D') { // Diagonal
            alignedA[align_idx] = SEQA[i-1];
            alignedB[align_idx] = SEQB[j-1];
            i--;
            j--;
        } else if (ptr[i * (BLEN + 1) + j] == 'U') { // Up
            alignedA[align_idx] = SEQA[i-1];
            alignedB[align_idx] = '_';
            i--;
        } else { // Left
            alignedA[align_idx] = '_';
            alignedB[align_idx] = SEQB[j-1];
            j--;
        }
        align_idx--;
    }

    // Pad the aligned sequences with underscores at the beginning
    for (int k = 0; k < align_idx + 1; k++) {
        alignedA[k] = '_';
        alignedB[k] = '_';
    }
}
