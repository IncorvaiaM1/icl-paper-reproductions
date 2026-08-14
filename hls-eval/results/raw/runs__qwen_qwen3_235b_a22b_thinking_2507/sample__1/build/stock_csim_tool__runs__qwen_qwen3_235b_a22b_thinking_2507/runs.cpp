
#include "runs.h"

void Runs(int *res_S, int *res_V, int epsilon[N]) {
    int S = 0;
    int V = 1; // Start with 1 run (first element always starts a run)
    int prev = epsilon[0];
    S += prev;

    for (int i = 1; i < N; i++) {
        #pragma HLS PIPELINE II=1
        S += epsilon[i];
        if (epsilon[i] != prev) {
            V++;
        }
        prev = epsilon[i];
    }

    *res_S = S;
    *res_V = V;
}
