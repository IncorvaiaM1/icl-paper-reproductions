
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
    char ptr[(ALEN + 1) * (BLEN + 1)]) {

    // Initialize first row
init_row:
    for (int j = 0; j <= BLEN; j++) {
        M[0 * (BLEN + 1) + j] = j * GAP_SCORE;
        ptr[0 * (BLEN + 1) + j] = 'L'; // Left
    }

    // Initialize first column
init_col:
    for (int i = 0; i <= ALEN; i++) {
        M[i * (BLEN + 1) + 0] = i * GAP_SCORE;
        ptr[i * (BLEN + 1) + 0] = 'U'; // Up
    }

    // Fill the matrix
fill_out:
    for (int i = 1; i <= ALEN; i++) {
    fill_in:
        for (int j = 1; j <= BLEN; j++) {
            int score_diag, score_up, score_left;
            char ptr_val;

            if (SEQA[i - 1] == SEQB[j - 1]) {
                score_diag = M[(i - 1) * (BLEN + 1) + (j - 1)] + MATCH_SCORE;
            } else {
                score_diag = M[(i - 1) * (BLEN + 1) + (j - 1)] + MISMATCH_SCORE;
            }

            score_up = M[(i - 1) * (BLEN + 1) + j] + GAP_SCORE;
            score_left = M[i * (BLEN + 1) + (j - 1)] + GAP_SCORE;

            // Determine the maximum score and corresponding pointer
            if (score_diag >= score_up && score_diag >= score_left) {
                M[i * (BLEN + 1) + j] = score_diag;
                ptr_val = 'D'; // Diagonal
            } else if (score_up >= score_diag && score_up >= score_left) {
                M[i * (BLEN + 1) + j] = score_up;
                ptr_val = 'U'; // Up
            } else {
                M[i * (BLEN + 1) + j] = score_left;
                ptr_val = 'L'; // Left
            }

            ptr[i * (BLEN + 1) + j] = ptr_val;
        }
    }

    // Traceback
    int i = ALEN;
    int j = BLEN;
    int k = ALEN + BLEN - 1;

trace:
    while (i > 0 || j > 0) {
        if (ptr[i * (BLEN + 1) + j] == 'D') {
            alignedA[k] = SEQA[i - 1];
            alignedB[k] = SEQB[j - 1];
            i--;
            j--;
        } else if (ptr[i * (BLEN + 1) + j] == 'U') {
            alignedA[k] = SEQA[i - 1];
            alignedB[k] = '_';
            i--;
        } else { // 'L'
            alignedA[k] = '_';
            alignedB[k] = SEQB[j - 1];
            j--;
        }
        k--;
    }

    // Pad the aligned sequences
pad_a:
    for (int p = 0; p < k + 1; p++) {
        alignedA[p] = '_';
    }

pad_b:
    for (int p = 0; p < k + 1; p++) {
        alignedB[p] = '_';
    }
}
