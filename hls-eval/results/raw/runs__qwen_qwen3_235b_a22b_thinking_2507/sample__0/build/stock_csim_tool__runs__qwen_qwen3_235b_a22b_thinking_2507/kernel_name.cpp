
#include "runs.h"

void Runs(int *res_S, int *res_V, int epsilon[N]) {
    int S = 0;
    int V = 1; // Initialize with 1 run (minimum for non-empty array)

    for (int i = 0; i < N; i++) {
        S += epsilon[i]; // Count ones
        
        // Check for transitions between consecutive elements (except last element)
        if (i < N-1 && epsilon[i] != epsilon[i+1]) {
            V++; // Increment run count when transition occurs
        }
    }

    *res_S = S;
    *res_V = V;
}
