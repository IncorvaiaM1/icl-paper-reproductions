
#include "runs.h"

void Runs(int *res_S, int *res_V, int epsilon[N]) {
    int S = 0;
    int V = 0;

    if (N <= 0) {
        *res_S = S;
        *res_V = V;
        return;
    }

    // Initialize S and V for the first element
    if (epsilon[0] == 1) {
        S = 1;
    }
    V = 1; // First element always starts the first run

    // Loop through the rest of the array
    for (int i = 1; i < N; i++) {
        // Count ones
        if (epsilon[i] == 1) {
            S++;
        }

        // Count runs: whenever current element differs from previous
        if (epsilon[i] != epsilon[i - 1]) {
            V++;
        }
    }

    *res_S = S;
    *res_V = V;
}
