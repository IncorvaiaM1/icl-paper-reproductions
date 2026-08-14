
#include "sort_merge.h"

static TYPE temp[SIZE];

void merge(TYPE a[SIZE], int start, int m, int stop) {
    int i, j, k;

merge_label1:
    for (i = start; i <= m; i++) {
        temp[i] = a[i];
    }

merge_label2:
    for (j = m + 1; j <= stop; j++) {
        temp[j] = a[j];
    }

    i = start;
    j = m + 1;

merge_label3:
    for (k = start; k <= stop; k++) {
        if (i > m) {
            a[k] = temp[j++];
        } else if (j > stop) {
            a[k] = temp[i++];
        } else if (temp[i] <= temp[j]) {
            a[k] = temp[i++];
        } else {
            a[k] = temp[j++];
        }
    }
}

void ms_mergesort(TYPE a[SIZE]) {
    int l, m, h, i;

    for (l = 1; l < SIZE; l = 2 * l) {
        for (m = 0; m < SIZE - 1; m += 2 * l) {
            h = m + 2 * l - 1;
            if (h >= SIZE) {
                h = SIZE - 1;
            }
            merge(a, m, m + l - 1, h);
        }
    }
}
