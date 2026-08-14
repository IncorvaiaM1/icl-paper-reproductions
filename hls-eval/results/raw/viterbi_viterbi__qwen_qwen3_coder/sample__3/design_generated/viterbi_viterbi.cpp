
#include "viterbi_viterbi.h"

int viterbi(
    tok_t obs[N_OBS],
    prob_t init[N_STATES],
    prob_t transition[N_STATES * N_STATES],
    prob_t emission[N_STATES * N_TOKENS],
    state_t path[N_OBS])
{
    // Likelihood array to store probabilities in -log space
    prob_t llike[N_OBS][N_STATES];
    // Path array to store backtracking information
    state_t path_arr[N_OBS][N_STATES];

    // Initialization: compute likelihood for the first observation
L_init:
    for (state_t s = 0; s < N_STATES; s++) {
        llike[0][s] = init[s] + emission[s * N_TOKENS + obs[0]];
        path_arr[0][s] = 0; // No previous state for t=0
    }

    // Iterative computation of probabilities over time
L_timestep:
    for (step_t t = 1; t < N_OBS; t++) {
L_curr_state:
        for (state_t curr = 0; curr < N_STATES; curr++) {
            prob_t min_prob = llike[t-1][0] + transition[0 * N_STATES + curr];
            state_t prev_state = 0;

L_prev_state:
            for (state_t prev = 1; prev < N_STATES; prev++) {
                prob_t prob = llike[t-1][prev] + transition[prev * N_STATES + curr];
                if (prob < min_prob) {
                    min_prob = prob;
                    prev_state = prev;
                }
            }

            llike[t][curr] = min_prob + emission[curr * N_TOKENS + obs[t]];
            path_arr[t][curr] = prev_state;
        }
    }

    // Identify the end state with the highest probability
    state_t end_state = 0;
    prob_t min_final_prob = llike[N_OBS-1][0];
L_end:
    for (state_t s = 1; s < N_STATES; s++) {
        if (llike[N_OBS-1][s] < min_final_prob) {
            min_final_prob = llike[N_OBS-1][s];
            end_state = s;
        }
    }

    // Backtrack to recover the full path
    path[N_OBS-1] = end_state;
L_backtrack:
    for (step_t t = N_OBS-2; t >= 0; t--) {
        path[t] = path_arr[t+1][path[t+1]];
    }

    return 0;
}
