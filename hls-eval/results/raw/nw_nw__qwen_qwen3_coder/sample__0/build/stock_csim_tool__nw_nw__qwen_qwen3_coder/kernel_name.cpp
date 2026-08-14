
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
init_row:
    for (int j = 0; j <= BLEN; j++) {
        M[j] = j * GAP_SCORE;
        ptr[j] = 'L'; // Left
    }

    // Initialize first column
init_col:
    for (int i = 1; i <= ALEN; i++) {
        M[i * (BLEN + 1)] = i * GAP_SCORE;
        ptr[i * (BLEN + 1)] = 'U'; // Up
    }

    // Fill the matrix
fill_out:
    for (int i = 1; i <= ALEN; i++) {
    fill_in:
        for (int j = 1; j <= BLEN; j++) {
            int diag_score = M[(i - 1) * (BLEN + 1) + (j - 1)];
            int up_score = M[(i - 1) * (BLEN + 1) + j];
            int left_score = M[i * (BLEN + 1) + (j - 1)];

            // Calculate match/mismatch score
            int score_diag = (SEQA[i - 1] == SEQB[j - 1]) ? MATCH_SCORE : MISMATCH_SCORE;
            diag_score += score_diag;

            // Add gap penalties
            up_score += GAP_SCORE;
            left_score += GAP_SCORE;

            // Find maximum score
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
    int aligned_index = ALEN + BLEN - 1;

trace:
    while (i > 0 || j > 0) {
        char direction = ptr[i * (BLEN + 1) + j];

        if (direction == 'D') { // Diagonal
            alignedA[aligned_index] = SEQA[i - 1];
            alignedB[aligned_index] = SEQB[j - 1];
            i--;
            j--;
        } else if (direction == 'U') { // Up
            alignedA[aligned_index] = SEQA[i - 1];
            alignedB[aligned_index] = '_';
            i--;
        } else { // Left
            alignedA[aligned_index] = '_';
            alignedB[aligned_index] = SEQB[j - 1];
            j--;
        }
        aligned_index--;
    }

    // Pad aligned sequences
pad_a:
    for (int k = 0; k < aligned_index + 1; k++) {
        alignedA[k] = '_';
    }

pad_b:
    for (int k = 0; k < aligned_index + 1; k++) {
        alignedB[k] = '_';
    }
}
