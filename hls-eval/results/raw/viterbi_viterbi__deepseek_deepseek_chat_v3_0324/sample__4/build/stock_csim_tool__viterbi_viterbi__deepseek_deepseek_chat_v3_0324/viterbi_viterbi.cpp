
#include "viterbi_viterbi.h"

int viterbi(
    tok_t obs[N_OBS],
    prob_t init[N_STATES],
    prob_t transition[N_STATES * N_STATES],
    prob_t emission[N_STATES * N_TOKENS],
    state_t path[N_OBS]) {

    // Likelihood array: N_OBS x N_STATES
    prob_t llike[N_OBS][N_STATES];
    // Backpointer array: N_OBS x N_STATES
    state_t backptr[N_OBS][N_STATES];

    // Initialization step
    INIT_LOOP:
    for (state_t s = 0; s < N_STATES; s++) {
        llike[0][s] = init[s] + emission[s * N_TOKENS + obs[0]];
        backptr[0][s] = 0;
    }

    // Iterative computation of probabilities
    TIME_LOOP:
    for (step_t t = 1; t < N_OBS; t++) {
        STATE_LOOP:
        for (state_t curr_s = 0; curr_s < N_STATES; curr_s++) {
            prob_t min_prob = llike[t-1][0] + transition[0 * N_STATES + curr_s];
            state_t min_state = 0;

            // Find the most likely previous state
            PREV_STATE_LOOP:
            for (state_t prev_s = 1; prev_s < N_STATES; prev_s++) {
                prob_t temp = llike[t-1][prev_s] + transition[prev_s * N_STATES + curr_s];
                if (temp < min_prob) {
                    min_prob = temp;
                    min_state = prev_s;
                }
            }

            // Update likelihood and backpointer
            llike[t][curr_s] = min_prob + emission[curr_s * N_TOKENS + obs[t]];
            backptr[t][curr_s] = min_state;
        }
    }

    // Find the end state with highest probability
    prob_t min_prob = llike[N_OBS-1][0];
    state_t min_state = 0;
    END_STATE_LOOP:
    for (state_t s = 1; s < N_STATES; s++) {
        if (llike[N_OBS-1][s] < min_prob) {
            min_prob = llike[N_OBS-1][s];
            min_state = s;
        }
    }

    // Backtrack to recover the full path
    path[N_OBS-1] = min_state;
    BACKTRACK_LOOP:
    for (step_t t = N_OBS-2; t >= 0; t--) {
        path[t] = backptr[t+1][path[t+1]];
    }

    return 0;
}
