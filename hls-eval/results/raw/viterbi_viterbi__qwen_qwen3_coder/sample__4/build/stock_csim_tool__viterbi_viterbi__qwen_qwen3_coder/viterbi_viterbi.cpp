
#include "viterbi_viterbi.h"

int viterbi(
    tok_t obs[N_OBS],
    prob_t init[N_STATES],
    prob_t transition[N_STATES * N_STATES],
    prob_t emission[N_STATES * N_TOKENS],
    state_t path[N_OBS]) {

    // Local storage for likelihoods and backtracking
    prob_t llike[N_OBS][N_STATES];
    state_t prev_state[N_OBS][N_STATES];

    // Initialization: compute likelihood for first observation
L_init:
    for (state_t s = 0; s < N_STATES; s++) {
        llike[0][s] = init[s] + emission[s * N_TOKENS + obs[0]];
        prev_state[0][s] = 0; // Not used for first step
    }

    // Iterative computation over time steps
L_timestep:
    for (step_t t = 1; t < N_OBS; t++) {
L_curr_state:
        for (state_t curr = 0; curr < N_STATES; curr++) {
            prob_t min_prob = llike[t-1][0] + transition[0 * N_STATES + curr];
            state_t min_state = 0;

L_prev_state:
            for (state_t prev = 1; prev < N_STATES; prev++) {
                prob_t prob = llike[t-1][prev] + transition[prev * N_STATES + curr];
                if (prob < min_prob) {
                    min_prob = prob;
                    min_state = prev;
                }
            }

            llike[t][curr] = min_prob + emission[curr * N_TOKENS + obs[t]];
            prev_state[t][curr] = min_state;
        }
    }

    // Find the most likely end state
    prob_t min_prob = llike[N_OBS-1][0];
    state_t min_state = 0;
L_end:
    for (state_t s = 1; s < N_STATES; s++) {
        if (llike[N_OBS-1][s] < min_prob) {
            min_prob = llike[N_OBS-1][s];
            min_state = s;
        }
    }

    // Backtrack to find the path
    path[N_OBS-1] = min_state;
L_backtrack:
    for (step_t t = N_OBS-1; t > 0; t--) {
        path[t-1] = prev_state[t][path[t]];
    }

    return 0;
}
