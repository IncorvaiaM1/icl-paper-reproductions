
#include "bfs_queue.h"

void bfs(
    node_t nodes[N_NODES],
    edge_t edges[N_EDGES],
    node_index_t starting_node,
    level_t level[N_NODES],
    edge_index_t level_counts[N_LEVELS]) {
    
    // Queue implementation using circular buffer
    node_index_t queue[N_NODES];
    int32_t head = 0;
    int32_t tail = 0;
    int32_t size = 0;
    
    // Initialize level array with maximum values
    INIT_LEVELS: for (node_index_t i = 0; i < N_NODES; i++) {
        level[i] = MAX_LEVEL;
    }
    
    // Initialize level_counts array with zero values
    INIT_HORIZONS: for (int32_t i = 0; i < N_LEVELS; i++) {
        level_counts[i] = 0;
    }
    
    // Set starting node level to 0 and add to queue
    level[starting_node] = 0;
    level_counts[0] = 1;
    queue[tail] = starting_node;
    tail = (tail + 1) % N_NODES;
    size++;
    
    level_t current_level = 0;
    
    // Process nodes in queue until empty
    LOOP_QUEUE: while (size > 0) {
        // Get node from front of queue
        node_index_t node = queue[head];
        head = (head + 1) % N_NODES;
        size--;
        
        // Get the current node's level
        level_t node_level = level[node];
        if (node_level > current_level) {
            current_level = node_level;
        }
        
        // Process all neighbors of current node
        edge_index_t edge_begin = nodes[node].edge_begin;
        edge_index_t edge_end = nodes[node].edge_end;
        
        LOOP_NEIGHBORS: for (edge_index_t i = edge_begin; i < edge_end; i++) {
            node_index_t neighbor = edges[i].dst;
            
            // Check if neighbor hasn't been visited yet
            if (level[neighbor] == MAX_LEVEL) {
                level_t new_level = node_level + 1;
                // Only process if within level limits
                if (new_level < N_LEVELS) {
                    level[neighbor] = new_level;
                    // Add to queue for processing
                    queue[tail] = neighbor;
                    tail = (tail + 1) % N_NODES;
                    size++;
                    
                    // Update level count
                    if (new_level < N_LEVELS) {
                        level_counts[new_level]++;
                    }
                }
            }
        }
    }
}
