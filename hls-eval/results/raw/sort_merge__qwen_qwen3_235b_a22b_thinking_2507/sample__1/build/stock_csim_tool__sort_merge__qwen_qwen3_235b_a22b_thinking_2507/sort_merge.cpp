
#include "sort_merge.h"

static void merge(TYPE a[SIZE], TYPE temp[SIZE], int start, int m, int stop) {
    merge_label1: for (int i = start; i < m; i++) {
        temp[i] = a[i];
    }
    merge_label2: for (int i = m; i < stop; i++) {
        temp[i] = a[i];
    }
    int i = start, j = m;
    merge_label3: for (int k = start; k < stop; k++) {
        if (j >= stop || (i < m && temp[i] <= temp[j])) {
            a[k] = temp[i];
            i++;
        } else {
            a[k] = temp[j];
            j++;
        }
    }
}

void ms_mergesort(TYPE a[SIZE]) {
    TYPE temp[SIZE];
    for (int width = 1; width < SIZE; width *= 2) {
        for (int i = 0; i < SIZE; i += 2 * width) {
            int mid = i + width;
            if (mid >= SIZE) {
                break;
            }
            int end = i + 2 * width;
            if (end > SIZE) {
                end = SIZE;
            }
            merge(a, temp, i, mid, end);
        }
    }
}
