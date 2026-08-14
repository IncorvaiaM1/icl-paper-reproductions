
#include "runs.h"

void Runs(int *res_S, int *res_V, int epsilon[N]) {
    int S = epsilon[0];
    int V = 1;
    int prev = epsilon[0];

    for (int i = 1; i < N; i++) {
#pragma HLS PIPELINE
        int current = epsilon[i];
        S += current;
        if (current != prev) {
            V++;
        }
        prev = current;
    }

    *res_S = S;
    *res_V = V;
}
