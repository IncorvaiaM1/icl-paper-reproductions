
#include "bfs_bulk.h"

void bfs(
    node_t nodes[N_NODES],
    edge_t edges[N_EDGES],
    node_index_t starting_node,
    level_t level[N_NODES],
    edge_index_t level_counts[N_LEVELS]) {

    // Initialize level array: set all levels to MAX_LEVEL except starting node
    level_t horizon;
    node_index_t n;
    edge_index_t e;

    for (n = 0; n < N_NODES; n++) {
        level[n] = MAX_LEVEL;
    }
    level[starting_node] = 0;

    // Initialize level_counts array
    for (horizon = 0; horizon < N_LEVELS; horizon++) {
        level_counts[horizon] = 0;
    }
    level_counts[0] = 1; // Starting node is at level 0

    // Iterate over levels (horizons)
    for (horizon = 0; horizon < N_LEVELS - 1; horizon++) {
        // Iterate over all nodes
        for (n = 0; n < N_NODES; n++) {
            // If node is at current level
            if (level[n] == horizon) {
                // Get the range of edges for this node
                edge_index_t tmp_begin = nodes[n].edge_begin;
                edge_index_t tmp_end = nodes[n].edge_end;

                // Iterate over neighbors
                for (e = tmp_begin; e < tmp_end; e++) {
                    node_index_t neighbor = edges[e].dst;
                    // If neighbor has not been visited yet
                    if (level[neighbor] == MAX_LEVEL) {
                        level[neighbor] = horizon + 1;
                        level_counts[horizon + 1]++;
                    }
                }
            }
        }
    }
}
