
#include "runs.h"

void Runs(int *res_S, int *res_V, int epsilon[N]) {
    #pragma HLS INTERFACE m_axi port=epsilon bundle=gmem
    #pragma HLS INTERFACE s_axilite port=res_S bundle=control
    #pragma HLS INTERFACE s_axilite port=res_V bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control

    int S = 0;
    int V = 0;
    int prev_val = epsilon[0];

    // Count the first element
    if (prev_val == 1) {
        S++;
    }
    V = 1; // At least one run exists

    for (int i = 1; i < N; i++) {
        #pragma HLS PIPELINE II=1
        int current_val = epsilon[i];
        
        // Count ones
        if (current_val == 1) {
            S++;
        }

        // Count runs (transitions between 0 and 1)
        if (current_val != prev_val) {
            V++;
        }
        prev_val = current_val;
    }

    *res_S = S;
    *res_V = V;
}
