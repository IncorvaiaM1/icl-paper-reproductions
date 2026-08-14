
#include "viterbi_viterbi.h"

int viterbi(
    tok_t obs[N_OBS],
    prob_t init[N_STATES],
    prob_t transition[N_STATES * N_STATES],
    prob_t emission[N_STATES * N_TOKENS],
    state_t path[N_OBS])
{
    prob_t llike[N_OBS][N_STATES];
    state_t back[N_OBS][N_STATES];

    // L_init: initialization for the first observation
    for (int s = 0; s < N_STATES; s++) {
        llike[0][s] = init[s] + emission[s * N_TOKENS + obs[0]];
    }

    // L_timestep: iterate over remaining observations
    for (int t = 1; t < N_OBS; t++) {
        // L_curr_state: iterate over current states
        for (int j = 0; j < N_STATES; j++) {
            // L_prev_state: find the most likely previous state
            prob_t min_prob = llike[t-1][0] + transition[0 * N_STATES + j] + emission[j * N_TOKENS + obs[t]];
            state_t min_state = 0;
            for (int i = 1; i < N_STATES; i++) {
                prob_t prob = llike[t-1][i] + transition[i * N_STATES + j] + emission[j * N_TOKENS + obs[t]];
                if (prob < min_prob) {
                    min_prob = prob;
                    min_state = i;
                }
            }
            llike[t][j] = min_prob;
            back[t][j] = min_state;
        }
    }

    // L_end: identify the most likely final state
    prob_t min_final = llike[N_OBS-1][0];
    state_t best_state = 0;
    for (int j = 1; j < N_STATES; j++) {
        if (llike[N_OBS-1][j] < min_final) {
            min_final = llike[N_OBS-1][j];
            best_state = j;
        }
    }
    path[N_OBS-1] = best_state;

    // L_backtrack: recover the most likely state sequence
    for (int t = N_OBS-2; t >= 0; t--) {
        best_state = back[t+1][best_state];
        path[t] = best_state;
    }

    return 0;
}
