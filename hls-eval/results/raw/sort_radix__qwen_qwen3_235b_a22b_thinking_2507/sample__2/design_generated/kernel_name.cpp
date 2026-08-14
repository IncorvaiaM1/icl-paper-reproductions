
#include "sort_radix.h"

void init(int bucket[BUCKETSIZE]) {
    for (int i = 0; i < BUCKETSIZE; i++) {
        bucket[i] = 0;
    }
}

void hist(int bucket[BUCKETSIZE], int a[SIZE], int exp) {
    for (int i = 0; i < SIZE; i++) {
        int digit = (a[i] >> exp) & MASK;
        int block_id = i / ELEMENTSPERBLOCK;
        int index = block_id * RADIXSIZE + digit;
        bucket[index]++;
    }
}

void local_scan(int bucket[BUCKETSIZE]) {
    for (int block_id = 0; block_id < NUMOFBLOCKS; block_id++) {
        int base = block_id * RADIXSIZE;
        for (int digit = 1; digit < RADIXSIZE; digit++) {
            bucket[base + digit] += bucket[base + digit - 1];
        }
    }
}

void sum_scan(int sum[SCAN_RADIX], int bucket[BUCKETSIZE]) {
    int total = 0;
    for (int block_id = 0; block_id < NUMOFBLOCKS; block_id++) {
        int base = block_id * RADIXSIZE;
        int block_total = bucket[base + RADIXSIZE - 1];
        bucket[base + RADIXSIZE - 1] = total;
        total += block_total;
    }
    sum[0] = total;
}

void last_step_scan(int bucket[BUCKETSIZE], int sum[SCAN_RADIX]) {
    for (int block_id = NUMOFBLOCKS - 1; block_id >= 0; block_id--) {
        int base = block_id * RADIXSIZE;
        int T = bucket[base + RADIXSIZE - 1];
        for (int digit = RADIXSIZE - 2; digit >= 0; digit--) {
            bucket[base + digit + 1] = bucket[base + digit] + T;
        }
        bucket[base] = T;
    }
}

void update(int b[SIZE], int bucket[BUCKETSIZE], int a[SIZE], int exp) {
    for (int i = 0; i < SIZE; i++) {
        int block_id = i / ELEMENTSPERBLOCK;
        int digit = (a[i] >> exp) & MASK;
        int base = block_id * RADIXSIZE;
        int index = bucket[base + digit];
        bucket[base + digit]++;
        b[index] = a[i];
    }
}

void ss_sort(int a[SIZE], int b[SIZE], int bucket[BUCKETSIZE], int sum[SCAN_RADIX]) {
    int *in = a;
    int *out = b;
    for (int exp = 0; exp < 32; exp += 2) {
        init(bucket);
        hist(bucket, in, exp);
        local_scan(bucket);
        sum_scan(sum, bucket);
        last_step_scan(bucket, sum);
        update(out, bucket, in, exp);
        int *temp = in;
        in = out;
        out = temp;
    }
}
