
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

    // Matrix indices
    int i, j;
    int index, index_up, index_left, index_diag;
    int score_diag, score_up, score_left;
    int max_score;

    // Traceback indices
    int tx, ty;
    int tindex;
    int aligned_index = 0;

    // Initialize first row
INIT_ROW: for (j = 0; j <= BLEN; j++) {
        M[j] = j * GAP_SCORE;
        ptr[j] = 'L'; // Left
    }

    // Initialize first column
INIT_COL: for (i = 1; i <= ALEN; i++) {
        M[i * (BLEN + 1)] = i * GAP_SCORE;
        ptr[i * (BLEN + 1)] = 'U'; // Up
    }

    // Fill matrices
FILL_OUT: for (i = 1; i <= ALEN; i++) {
FILL_IN:     for (j = 1; j <= BLEN; j++) {
                index = i * (BLEN + 1) + j;
                index_up = (i - 1) * (BLEN + 1) + j;
                index_left = i * (BLEN + 1) + (j - 1);
                index_diag = (i - 1) * (BLEN + 1) + (j - 1);

                // Compute scores
                if (SEQA[i - 1] == SEQB[j - 1]) {
                    score_diag = M[index_diag] + MATCH_SCORE;
                } else {
                    score_diag = M[index_diag] + MISMATCH_SCORE;
                }
                score_up = M[index_up] + GAP_SCORE;
                score_left = M[index_left] + GAP_SCORE;

                // Find maximum score
                max_score = score_diag;
                ptr[index] = 'D'; // Diagonal

                if (score_up > max_score) {
                    max_score = score_up;
                    ptr[index] = 'U'; // Up
                }

                if (score_left > max_score) {
                    max_score = score_left;
                    ptr[index] = 'L'; // Left
                }

                M[index] = max_score;
            }
        }

    // Traceback
    tx = ALEN;
    ty = BLEN;
TRACE: while (tx > 0 || ty > 0) {
            tindex = tx * (BLEN + 1) + ty;

            if (ptr[tindex] == 'D') { // Diagonal
                alignedA[aligned_index] = SEQA[tx - 1];
                alignedB[aligned_index] = SEQB[ty - 1];
                tx--;
                ty--;
            } else if (ptr[tindex] == 'U') { // Up
                alignedA[aligned_index] = SEQA[tx - 1];
                alignedB[aligned_index] = '_';
                tx--;
            } else { // Left
                alignedA[aligned_index] = '_';
                alignedB[aligned_index] = SEQB[ty - 1];
                ty--;
            }
            aligned_index++;
        }

    // Pad sequences to fixed length
PAD_A: for (i = aligned_index; i < ALEN + BLEN; i++) {
            alignedA[i] = '_';
        }

PAD_B: for (i = aligned_index; i < ALEN + BLEN; i++) {
            alignedB[i] = '_';
        }

    // Reverse sequences
    char tempA, tempB;
REVERSE: for (i = 0; i < aligned_index / 2; i++) {
            tempA = alignedA[i];
            tempB = alignedB[i];
            alignedA[i] = alignedA[aligned_index - 1 - i];
            alignedB[i] = alignedB[aligned_index - 1 - i];
            alignedA[aligned_index - 1 - i] = tempA;
            alignedB[aligned_index - 1 - i] = tempB;
        }
}
