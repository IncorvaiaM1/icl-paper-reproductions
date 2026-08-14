
#include "runs.h"

void Runs(int *res_S, int *res_V, int epsilon[N]) {
    #pragma HLS INTERFACE m_axi port=epsilon offset=slave bundle=gmem
    #pragma HLS INTERFACE m_axi port=res_S offset=slave bundle=gmem
    #pragma HLS INTERFACE m_axi port=res_V offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=return bundle=control

    int S = 0;
    int V = 1; // Start with 1 run since first element counts as first run
    int prev_val = epsilon[0];

    // Count ones and runs
    for (int i = 0; i < N; i++) {
        #pragma HLS PIPELINE II=1
        if (epsilon[i] == 1) {
            S++;
        }

        if (i > 0 && epsilon[i] != prev_val) {
            V++;
            prev_val = epsilon[i];
        }
    }

    *res_S = S;
    *res_V = V;
}
