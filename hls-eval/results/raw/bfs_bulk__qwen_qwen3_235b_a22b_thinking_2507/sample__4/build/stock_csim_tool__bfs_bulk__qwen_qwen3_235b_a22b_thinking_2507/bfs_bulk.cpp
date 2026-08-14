
#include "bfs_bulk.h"

void bfs(node_t nodes[N_NODES], edge_t edges[N_EDGES], node_index_t starting_node, level_t level[N_NODES], edge_index_t level_counts[N_LEVELS]) {
    // Initialize the starting node level to 0
    level[starting_node] = 0;
    
    // Initialize all level counts to 0
    for (int i = 0; i < N_LEVELS; i++) {
        level_counts[i] = 0;
    }
    // Starting node is at level 0
    level_counts[0] = 1;
    
    // Iterate through each level (horizon) of the BFS
    for (int horizon = 0; horizon < N_LEVELS; horizon++) {
        // Temporary array to mark nodes visited in the next level during this horizon
        bool next_level_visited[N_NODES] = {false};
        
        // Check all nodes to find those at the current horizon
        for (int n = 0; n < N_NODES; n++) {
            if (level[n] == horizon) {
                // Process all edges from this node
                edge_index_t tmp_begin = nodes[n].edge_begin;
                edge_index_t tmp_end = nodes[n].edge_end;
                for (edge_index_t e = tmp_begin; e < tmp_end; e++) {
                    node_index_t neighbor = edges[e].dst;
                    // Only consider unvisited neighbors (those with MAX_LEVEL)
                    if (level[neighbor] == MAX_LEVEL) {
                        next_level_visited[neighbor] = true;
                    }
                }
            }
        }
        
        // Count and update the level for all newly visited nodes
        edge_index_t count = 0;
        for (int i = 0; i < N_NODES; i++) {
            if (next_level_visited[i]) {
                level[i] = horizon + 1;
                count++;
            }
        }
        
        // Store the count for the next level if within bounds
        if (horizon + 1 < N_LEVELS) {
            level_counts[horizon + 1] = count;
        }
    }
}
