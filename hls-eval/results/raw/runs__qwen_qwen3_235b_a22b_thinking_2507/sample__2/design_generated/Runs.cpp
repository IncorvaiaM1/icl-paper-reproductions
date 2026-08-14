
#include "runs.h"

void Runs(int *res_S, int *res_V, int epsilon[N]) {
    int S = 0;
    int V = 1; // Start with 1 run (first element always starts a run)

    for (int i = 0; i < N; i++) {
        S += epsilon[i];
        
        // Check for value transitions starting from the second element
        if (i > 0 && epsilon[i] != epsilon[i-1]) {
            V++;
        }
    }

    *res_S = S;
    *res_V = V;
}
