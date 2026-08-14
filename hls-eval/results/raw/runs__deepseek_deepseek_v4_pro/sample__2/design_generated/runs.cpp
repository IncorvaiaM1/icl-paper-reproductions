
#include "runs.h"

void Runs(int *res_S, int *res_V, int epsilon[N]) {
    int count_ones = 0;
    int count_runs = 0;
    int prev = epsilon[0];

    // The first element always starts the first run
    count_ones = prev;
    count_runs = 1;

    for (int i = 1; i < N; i++) {
#pragma HLS PIPELINE II=1
        int curr = epsilon[i];
        count_ones += curr;

        if (curr != prev) {
            count_runs++;
        }

        prev = curr;
    }

    *res_S = count_ones;
    *res_V = count_runs;
}
