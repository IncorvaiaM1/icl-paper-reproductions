
#include "sort_radix.h"

void init(int bucket[BUCKETSIZE]) {
    for (int i = 0; i < BUCKETSIZE; i++) {
        bucket[i] = 0;
    }
}

void hist(int bucket[BUCKETSIZE], int a[SIZE], int exp) {
    for (int i = 0; i < SIZE; i++) {
        int block_id = i / ELEMENTSPERBLOCK;
        int radix = (a[i] >> exp) & MASK;
        bucket[block_id * RADIXSIZE + radix]++;
    }
}

void local_scan(int bucket[BUCKETSIZE]) {
    for (int block_id = 0; block_id < NUMOFBLOCKS; block_id++) {
        for (int radix = 1; radix < RADIXSIZE; radix++) {
            bucket[block_id * RADIXSIZE + radix] += bucket[block_id * RADIXSIZE + radix - 1];
        }
    }
}

void sum_scan(int sum[SCAN_RADIX], int bucket[BUCKETSIZE]) {
    for (int group = 0; group < SCAN_RADIX; group++) {
        int total = 0;
        for (int i = 0; i < 4; i++) {
            int block_id = group * 4 + i;
            total += bucket[block_id * RADIXSIZE + (RADIXSIZE - 1)];
        }
        sum[group] = total;
    }

    int prefix = 0;
    for (int group = 0; group < SCAN_RADIX; group++) {
        int temp = sum[group];
        sum[group] = prefix;
        prefix += temp;
    }
}

void last_step_scan(int bucket[BUCKETSIZE], int sum[SCAN_RADIX]) {
    for (int group = 0; group < SCAN_RADIX; group++) {
        int base = sum[group];
        int block_id0 = group * 4;
        
        int T0 = bucket[block_id0 * RADIXSIZE + 3];
        int T1 = bucket[(block_id0 + 1) * RADIXSIZE + 3];
        int T2 = bucket[(block_id0 + 2) * RADIXSIZE + 3];
        
        for (int radix = 0; radix < RADIXSIZE; radix++) {
            bucket[block_id0 * RADIXSIZE + radix] += base;
        }
        
        for (int radix = 0; radix < RADIXSIZE; radix++) {
            bucket[(block_id0 + 1) * RADIXSIZE + radix] += base + T0;
        }
        
        for (int radix = 0; radix < RADIXSIZE; radix++) {
            bucket[(block_id0 + 2) * RADIXSIZE + radix] += base + T0 + T1;
        }
        
        for (int radix = 0; radix < RADIXSIZE; radix++) {
            bucket[(block_id0 + 3) * RADIXSIZE + radix] += base + T0 + T1 + T2;
        }
    }
}

void update(int b[SIZE], int bucket[BUCKETSIZE], int a[SIZE], int exp) {
    for (int i = 0; i < SIZE; i++) {
        int block_id = i / ELEMENTSPERBLOCK;
        int radix = (a[i] >> exp) & MASK;
        int pos = bucket[block_id * RADIXSIZE + radix];
        b[pos] = a[i];
        bucket[block_id * RADIXSIZE + radix] = pos + 1;
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
