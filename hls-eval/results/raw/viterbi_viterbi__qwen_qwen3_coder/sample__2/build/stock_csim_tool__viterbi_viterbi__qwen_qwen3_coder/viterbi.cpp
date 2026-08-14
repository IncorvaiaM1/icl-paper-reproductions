
#include "viterbi_viterbi.h"

int viterbi(
    tok_t obs[N_OBS],
    prob_t init[N_STATES],
    prob_t transition[N_STATES * N_STATES],
    prob_t emission[N_STATES * N_TOKENS],
    state_t path[N_OBS]) {

    // Declare likelihood and path arrays
    prob_t llike[N_OBS][N_STATES];
    state_t prev[N_OBS][N_STATES];

    // Initialization: compute likelihood for the first observation
L_init:
    for (state_t s = 0; s < N_STATES; s++) {
        llike[0][s] = init[s] + emission[s * N_TOKENS + obs[0]];
        prev[0][s] = 0;
    }

    // Iterative computation of probabilities
L_timestep:
    for (step_t t = 1; t < N_OBS; t++) {
L_curr_state:
        for (state_t curr = 0; curr < N_STATES; curr++) {
            prob_t min_prob = llike[t - 1][0] + transition[0 * N_STATES + curr];
            state_t min_state = 0;

L_prev_state:
            for (state_t prev_state = 1; prev_state < N_STATES; prev_state++) {
                prob_t prob = llike[t - 1][prev_state] + transition[prev_state * N_STATES + curr];
                if (prob < min_prob) {
                    min_prob = prob;
                    min_state = prev_state;
                }
            }

            llike[t][curr] = min_prob + emission[curr * N_TOKENS + obs[t]];
            prev[t][curr] = min_state;
        }
    }

    // Identify the end state with the highest probability (minimum in -log space)
    prob_t min_prob_end = llike[N_OBS - 1][0];
    state_t min_state_end = 0;

L_end:
    for (state_t s = 1; s < N_STATES; s++) {
        if (llike[N_OBS - 1][s] < min_prob_end) {
            min_prob_end = llike[N_OBS - 1][s];
            min_state_end = s;
        }
    }

    // Backtrack to recover the full path
    path[N_OBS - 1] = min_state_end;

L_backtrack:
    for (step_t t = N_OBS - 2; t >= 0; t--) {
        path[t] = prev[t + 1][path[t + 1]];
    }

    return 0;
}
