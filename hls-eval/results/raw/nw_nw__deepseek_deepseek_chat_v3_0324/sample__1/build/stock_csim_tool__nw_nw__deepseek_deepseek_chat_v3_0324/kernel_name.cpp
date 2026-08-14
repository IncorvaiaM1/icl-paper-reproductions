
#include "nw_nw.h"

#define MATCH_SCORE 1
#define MISMATCH_SCORE -1
#define GAP_SCORE -1

inline int get_index(int i, int j) {
    return i * (BLEN + 1) + j;
}

inline int score(char a, char b) {
    return (a == b) ? MATCH_SCORE : MISMATCH_SCORE;
}

void needwun(
    char SEQA[ALEN],
    char SEQB[BLEN],
    char alignedA[ALEN + BLEN],
    char alignedB[ALEN + BLEN],
    int M[(ALEN + 1) * (BLEN + 1)],
    char ptr[(ALEN + 1) * (BLEN + 1)]) {

    // Initialize first row and column of M and ptr
    for (int i = 0; i <= ALEN; i++) {
        M[get_index(i, 0)] = i * GAP_SCORE;
        ptr[get_index(i, 0)] = 'U';
    }
    for (int j = 0; j <= BLEN; j++) {
        M[get_index(0, j)] = j * GAP_SCORE;
        ptr[get_index(0, j)] = 'L';
    }
    ptr[get_index(0, 0)] = 'D';

    // Fill the M matrix and ptr matrix
    for (int i = 1; i <= ALEN; i++) {
        for (int j = 1; j <= BLEN; j++) {
            int match = M[get_index(i-1, j-1)] + score(SEQA[i-1], SEQB[j-1]);
            int del = M[get_index(i-1, j)] + GAP_SCORE;
            int ins = M[get_index(i, j-1)] + GAP_SCORE;

            if (match >= del && match >= ins) {
                M[get_index(i, j)] = match;
                ptr[get_index(i, j)] = 'D';
            } else if (del >= ins) {
                M[get_index(i, j)] = del;
                ptr[get_index(i, j)] = 'U';
            } else {
                M[get_index(i, j)] = ins;
                ptr[get_index(i, j)] = 'L';
            }
        }
    }

    // Traceback to find the optimal alignment
    int i = ALEN;
    int j = BLEN;
    int pos = ALEN + BLEN - 1;

    while (i > 0 || j > 0) {
        if (ptr[get_index(i, j)] == 'D') {
            alignedA[pos] = SEQA[i-1];
            alignedB[pos] = SEQB[j-1];
            i--;
            j--;
        } else if (ptr[get_index(i, j)] == 'U') {
            alignedA[pos] = SEQA[i-1];
            alignedB[pos] = '-';
            i--;
        } else { // 'L'
            alignedA[pos] = '-';
            alignedB[pos] = SEQB[j-1];
            j--;
        }
        pos--;
    }

    // Pad the remaining positions with underscores
    while (pos >= 0) {
        alignedA[pos] = '_';
        alignedB[pos] = '_';
        pos--;
    }
}
