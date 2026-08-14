
#include "sort_radix.h"

void init(int bucket[BUCKETSIZE]) {
    for (int i = 0; i < BUCKETSIZE; i++) {
        bucket[i] = 0;
    }
}

void hist(int bucket[BUCKETSIZE], int a[SIZE], int exp) {
    for (int i = 0; i < SIZE; i++) {
        int index = (a[i] >> exp) & MASK;
        int block_id = i / ELEMENTSPERBLOCK;
        bucket[block_id * RADIXSIZE + index]++;
    }
}

void local_scan(int bucket[BUCKETSIZE]) {
    for (int i = 0; i < NUMOFBLOCKS; i++) {
        int offset = i * RADIXSIZE;
        for (int j = 1; j < RADIXSIZE; j++) {
            bucket[offset + j] += bucket[offset + j - 1];
        }
    }
}

void sum_scan(int sum[SCAN_RADIX], int bucket[BUCKETSIZE]) {
    for (int i = 0; i < SCAN_RADIX; i++) {
        sum[i] = 0;
        for (int j = 0; j < NUMOFBLOCKS; j++) {
            sum[i] += bucket[j * RADIXSIZE + i];
        }
    }
}

void last_step_scan(int bucket[BUCKETSIZE], int sum[SCAN_RADIX]) {
    for (int i = 0; i < NUMOFBLOCKS; i++) {
        int offset = i * RADIXSIZE;
        for (int j = 0; j < RADIXSIZE; j++) {
            bucket[offset + j] += sum[j];
        }
    }
}

void update(int b[SIZE], int bucket[BUCKETSIZE], int a[SIZE], int exp) {
    for (int i = SIZE - 1; i >= 0; i--) {
        int index = (a[i] >> exp) & MASK;
        int block_id = i / ELEMENTSPERBLOCK;
        int pos = --bucket[block_id * RADIXSIZE + index];
        b[pos] = a[i];
    }
}

void ss_sort(int a[SIZE], int b[SIZE], int bucket[BUCKETSIZE], int sum[SCAN_RADIX]) {
    int exp = 0;
    for (int pass = 0; pass < 16; pass++) { // 16 passes for 32-bit integers (2 bits per pass)
        init(bucket);
        hist(bucket, a, exp);
        local_scan(bucket);
        sum_scan(sum, bucket);
        last_step_scan(bucket, sum);
        update(b, bucket, a, exp);

        // Swap arrays for next iteration
        int* temp = a;
        a = b;
        b = temp;

        exp += 2; // Move to next 2-bit digit
    }
}
