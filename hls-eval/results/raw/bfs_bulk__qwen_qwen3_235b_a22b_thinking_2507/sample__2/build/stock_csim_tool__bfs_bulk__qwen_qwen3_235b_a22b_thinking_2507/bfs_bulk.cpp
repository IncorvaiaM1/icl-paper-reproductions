
#include "bfs_bulk.h"

void bfs(node_t nodes[N_NODES], edge_t edges[N_EDGES], node_index_t starting_node, level_t level[N_NODES], edge_index_t level_counts[N_LEVELS]) {
    // Initialize all nodes to unvisited (MAX_LEVEL)
    for (int i = 0; i < N_NODES; i++) {
        level[i] = MAX_LEVEL;
    }
    // Set starting node level to 0
    level[starting_node] = 0;
    
    // Initialize level counts to 0
    for (int i = 0; i < N_LEVELS; i++) {
        level_counts[i] = 0;
    }
    // Starting node is at level 0
    level_counts[0] = 1;
    
    // Iterate through each level (horizon) up to maximum levels
    for (int horizon = 0; horizon < N_LEVELS; horizon++) {
        edge_index_t next_level_count = 0;
        
        // Process all nodes to find those at current horizon
        for (int n = 0; n < N_NODES; n++) {
            // Only process nodes at current level
            if (level[n] == horizon) {
                // Get edge range for this node
                edge_index_t begin = nodes[n].edge_begin;
                edge_index_t end = nodes[n].edge_end;
                
                // Process all edges of the node
                for (edge_index_t e = begin; e < end; e++) {
                    node_index_t neighbor = edges[e].dst;
                    
                    // Only process unvisited neighbors
                    if (level[neighbor] == MAX_LEVEL) {
                        // Mark neighbor with next level
                        level[neighbor] = horizon + 1;
                        
                        // Count for next level (if within bounds)
                        if (horizon < N_LEVELS - 1) {
                            next_level_count++;
                        }
                    }
                }
            }
        }
        
        // Update level counts for next level (if within array bounds)
        if (horizon < N_LEVELS - 1) {
            level_counts[horizon + 1] = next_level_count;
        }
    }
}
