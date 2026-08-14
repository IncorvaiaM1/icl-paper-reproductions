
#include "viterbi_viterbi.h"
#include <cfloat>

// Large constant representing infinity in -log space.
// Since probabilities are in [0,1], -log(p) >= 0.
// DBL_MAX is safe for comparisons.
#define INF DBL_MAX

int viterbi(
    tok_t obs[N_OBS],
    prob_t init[N_STATES],
    prob_t transition[N_STATES * N_STATES],
    prob_t emission[N_STATES * N_TOKENS],
    state_t path[N_OBS])
{
    // L_init: likelihood array for all time steps and states
    prob_t llike[N_OBS][N_STATES];
    // Backpointer array to reconstruct the most likely path
    state_t back[N_OBS][N_STATES];

    // --- Initialization (t = 0) ---
    // L_init: compute likelihood for the first observation
    for (int i = 0; i < N_STATES; i++) {
        // -log(P(state=i) * P(obs[0] | state=i))
        // = init[i] + emission[i][obs[0]]
        llike[0][i] = init[i] + emission[i][obs[0]];
        // No predecessor for the first step
        back[0][i] = 0;
    }

    // --- Recursion (t = 1 .. N_OBS-1) ---
    // L_timestep: iterate over remaining observations
    for (int t = 1; t < N_OBS; t++) {
        // L_curr_state: for each current state j
        for (int j = 0; j < N_STATES; j++) {
            prob_t min_prob = INF;
            state_t best_i = 0;

            // L_prev_state: find the best previous state i
            for (int i = 0; i < N_STATES; i++) {
                // transition matrix is stored row-major: index = i*N_STATES + j
                prob_t prob = llike[t-1][i] + transition[i * N_STATES + j];
                if (prob < min_prob) {
                    min_prob = prob;
                    best_i = i;
                }
            }

            // Add emission probability for current observation
            llike[t][j] = min_prob + emission[j][obs[t]];
            back[t][j] = best_i;
        }
    }

    // --- Termination ---
    // L_end: find the state with the highest probability (smallest -log prob)
    prob_t min_final = INF;
    state_t best_last = 0;
    for (int i = 0; i < N_STATES; i++) {
        if (llike[N_OBS - 1][i] < min_final) {
            min_final = llike[N_OBS - 1][i];
            best_last = i;
        }
    }
    path[N_OBS - 1] = best_last;

    // --- Backtracking ---
    // L_backtrack: recover the most likely state sequence
    for (int t = N_OBS - 2; t >= 0; t--) {
        // The best state at time t is the predecessor stored for the
        // best state at time t+1
        path[t] = back[t + 1][path[t + 1]];
    }

    // Success
    return 0;
}
