
#include "viterbi_viterbi.h"

int viterbi(tok_t obs[N_OBS], prob_t init[N_STATES], prob_t transition[N_STATES * N_STATES], prob_t emission[N_STATES * N_TOKENS], state_t path[N_OBS]) {
    // Declare the likelihood and backpointer arrays
    prob_t llike[N_OBS][N_STATES];
    state_t backptr[N_OBS][N_STATES];

    // Initialization step (t = 0)
    for (state_t j = 0; j < N_STATES; j++) {
        llike[0][j] = init[j] + emission[j * N_TOKENS + obs[0]];
    }

    // Iterative step for t = 1 to N_OBS-1
    for (step_t t = 1; t < N_OBS; t++) {
        for (state_t j = 0; j < N_STATES; j++) {
            // Start with state 0
            prob_t min_val = llike[t-1][0] + transition[0 * N_STATES + j];
            state_t min_state = 0;
            // Check states 1 to N_STATES-1
            for (state_t i = 1; i < N_STATES; i++) {
                prob_t candidate = llike[t-1][i] + transition[i * N_STATES + j];
                if (candidate < min_val) {
                    min_val = candidate;
                    min_state = i;
                }
            }
            llike[t][j] = min_val + emission[j * N_TOKENS + obs[t]];
            backptr[t][j] = min_state;
        }
    }

    // Find the end state (at t = N_OBS-1)
    prob_t min_val = llike[N_OBS-1][0];
    state_t end_state = 0;
    for (state_t j = 1; j < N_STATES; j++) {
        if (llike[N_OBS-1][j] < min_val) {
            min_val = llike[N_OBS-1][j];
            end_state = j;
        }
    }
    path[N_OBS-1] = end_state;

    // Backtrack to find the full path
    for (step_t t = N_OBS-2; t >= 0; t--) {
        path[t] = backptr[t+1][path[t+1]];
    }

    return 0;
}
