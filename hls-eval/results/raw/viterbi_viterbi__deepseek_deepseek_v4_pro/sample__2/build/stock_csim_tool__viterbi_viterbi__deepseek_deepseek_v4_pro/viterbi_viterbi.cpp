
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

    step_t t, i, j;
    prob_t min_val, val;
    state_t min_state;

    // L_init: initialization for the first observation
    for (i = 0; i < N_STATES; i++) {
        llike[0][i] = init[i] + emission[i * N_TOKENS + obs[0]];
        back[0][i] = 0;
    }

    // L_timestep: iterate over remaining observations
    for (t = 1; t < N_OBS; t++) {
        // L_curr_state: for each current state
        for (j = 0; j < N_STATES; j++) {
            // L_prev_state: find the most probable previous state
            min_val = llike[t-1][0] + transition[0 * N_STATES + j];
            min_state = 0;
            for (i = 1; i < N_STATES; i++) {
                val = llike[t-1][i] + transition[i * N_STATES + j];
                if (val < min_val) {
                    min_val = val;
                    min_state = i;
                }
            }
            llike[t][j] = min_val + emission[j * N_TOKENS + obs[t]];
            back[t][j] = min_state;
        }
    }

    // L_end: identify the final state with the highest probability
    min_val = llike[N_OBS-1][0];
    min_state = 0;
    for (j = 1; j < N_STATES; j++) {
        if (llike[N_OBS-1][j] < min_val) {
            min_val = llike[N_OBS-1][j];
            min_state = j;
        }
    }
    path[N_OBS-1] = min_state;

    // L_backtrack: recover the full state sequence
    for (t = N_OBS-2; t >= 0; t--) {
        path[t] = back[t+1][path[t+1]];
    }

    return 0;
}
