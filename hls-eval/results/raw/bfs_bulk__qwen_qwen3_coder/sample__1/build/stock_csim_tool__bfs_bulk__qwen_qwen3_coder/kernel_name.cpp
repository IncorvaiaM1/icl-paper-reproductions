
#include "bfs_bulk.h"

void bfs(
    node_t nodes[N_NODES],
    edge_t edges[N_EDGES],
    node_index_t starting_node,
    level_t level[N_NODES],
    edge_index_t level_counts[N_LEVELS]) {

    // Initialize level array: set all levels to MAX_LEVEL (unvisited)
    for (node_index_t n = 0; n < N_NODES; n++) {
        level[n] = MAX_LEVEL;
    }

    // Initialize level_counts array to zero
    for (edge_index_t l = 0; l < N_LEVELS; l++) {
        level_counts[l] = 0;
    }

    // Set the starting node's level to 0 and update level count
    level[starting_node] = 0;
    level_counts[0] = 1;

    // Iterate through each level (horizon)
    for (edge_index_t horizon = 0; horizon < N_LEVELS - 1; horizon++) {
        edge_index_t current_level_count = level_counts[horizon];
        if (current_level_count == 0) {
            // No more nodes to process at this level, exit early
            break;
        }
        level_counts[horizon + 1] = 0;

        // Process all nodes at the current level
        for (node_index_t n = 0; n < N_NODES; n++) {
            if (level[n] == horizon) {
                edge_index_t tmp_begin = nodes[n].edge_begin;
                edge_index_t tmp_end = nodes[n].edge_end;

                // Traverse all neighbors of node n
                for (edge_index_t e = tmp_begin; e < tmp_end; e++) {
                    node_index_t v = edges[e].dst;

                    // If neighbor v is unvisited, assign it to the next level
                    if (level[v] == MAX_LEVEL) {
                        level[v] = horizon + 1;
                        level_counts[horizon + 1]++;
                    }
                }
            }
        }
    }
}
