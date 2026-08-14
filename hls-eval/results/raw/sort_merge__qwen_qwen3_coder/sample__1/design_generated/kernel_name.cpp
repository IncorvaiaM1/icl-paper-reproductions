
#include "sort_merge.h"

// Temporary array for merging
static TYPE temp[SIZE];

// Merge function to merge two sorted subarrays
void merge(TYPE a[SIZE], int start, int m, int stop) {
    int i, j, k;

    // Copy left half to temp array
merge_label1:
    for (i = start; i <= m; i++) {
        temp[i] = a[i];
    }

    // Copy right half to temp array
merge_label2:
    for (j = m + 1; j <= stop; j++) {
        temp[j] = a[j];
    }

    // Merge the two halves back into a
    i = start;
    j = m + 1;
    k = start;

merge_label3:
    while (i <= m && j <= stop) {
        if (temp[i] <= temp[j]) {
            a[k] = temp[i];
            i++;
        } else {
            a[k] = temp[j];
            j++;
        }
        k++;
    }

    // Copy remaining elements from left half
    while (i <= m) {
        a[k] = temp[i];
        i++;
        k++;
    }

    // Copy remaining elements from right half
    while (j <= stop) {
        a[k] = temp[j];
        j++;
        k++;
    }
}

// Top-level function for merge sort
void ms_mergesort(TYPE a[SIZE]) {
    int lsz;   // Size of subarrays to be merged
    int start; // Starting index of subarray
    int m;     // Middle index
    int stop;  // Ending index of subarray

    // Iterate over subarray sizes (powers of 2)
    for (lsz = 1; lsz < SIZE; lsz *= 2) {
        // Iterate over each pair of subarrays of size lsz
        for (start = 0; start < SIZE - lsz; start += 2 * lsz) {
            m = start + lsz - 1;
            stop = start + 2 * lsz - 1;
            if (stop >= SIZE) {
                stop = SIZE - 1;
            }
            merge(a, start, m, stop);
        }
    }
}
