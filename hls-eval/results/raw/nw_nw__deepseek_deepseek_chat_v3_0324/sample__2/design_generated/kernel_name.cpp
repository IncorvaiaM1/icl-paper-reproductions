
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

    // Initialize first row and column of M and ptr
    M[0] = 0;
    ptr[0] = 0;
    for (int i = 1; i <= ALEN; i++) {
        M[i * (BLEN + 1)] = i * GAP_SCORE;
        ptr[i * (BLEN + 1)] = 'u';
    }
    for (int j = 1; j <= BLEN; j++) {
        M[j] = j * GAP_SCORE;
        ptr[j] = 'l';
    }

    // Fill the M matrix
    for (int i = 1; i <= ALEN; i++) {
        for (int j = 1; j <= BLEN; j++) {
            int score_diag = M[(i - 1) * (BLEN + 1) + (j - 1)] + 
                            (SEQA[i - 1] == SEQB[j - 1] ? MATCH_SCORE : MISMATCH_SCORE);
            int score_up = M[(i - 1) * (BLEN + 1) + j] + GAP_SCORE;
            int score_left = M[i * (BLEN + 1) + (j - 1)] + GAP_SCORE;

            if (score_diag >= score_up && score_diag >= score_left) {
                M[i * (BLEN + 1) + j] = score_diag;
                ptr[i * (BLEN + 1) + j] = 'd';
            } else if (score_up >= score_left) {
                M[i * (BLEN + 1) + j] = score_up;
                ptr[i * (BLEN + 1) + j] = 'u';
            } else {
                M[i * (BLEN + 1) + j] = score_left;
                ptr[i * (BLEN + 1) + j] = 'l';
            }
        }
    }

    // Traceback
    int i = ALEN;
    int j = BLEN;
    int a_pos = ALEN + BLEN - 1;
    int b_pos = ALEN + BLEN - 1;

    while (i > 0 || j > 0) {
        if (ptr[i * (BLEN + 1) + j] == 'd') {
            alignedA[a_pos--] = SEQA[i - 1];
            alignedB[b_pos--] = SEQB[j - 1];
            i--;
            j--;
        } else if (ptr[i * (BLEN + 1) + j] == 'u') {
            alignedA[a_pos--] = SEQA[i - 1];
            alignedB[b_pos--] = '_';
            i--;
        } else { // 'l'
            alignedA[a_pos--] = '_';
            alignedB[b_pos--] = SEQB[j - 1];
            j--;
        }
    }

    // Pad the beginning with underscores
    while (a_pos >= 0) {
        alignedA[a_pos--] = '_';
        alignedB[b_pos--] = '_';
    }
}
