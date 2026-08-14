
#include "bfs_queue.h"

void bfs(
    node_t nodes[N_NODES],
    edge_t edges[N_EDGES],
    node_index_t starting_node,
    level_t level[N_NODES],
    edge_index_t level_counts[N_LEVELS]) {
    
    // Queue implementation using circular buffer
    node_index_t queue[N_NODES];
    int64_t head = 0;
    int64_t tail = 0;
    
    // Initialize levels to MAX_LEVEL
    INIT_LEVELS: for (int64_t i = 0; i < N_NODES; i++) {
        level[i] = MAX_LEVEL;
    }
    
    // Initialize level counts to 0
    INIT_HORIZONS: for (int64_t i = 0; i < N_LEVELS; i++) {
        level_counts[i] = 0;
    }
    
    // Set starting node level to 0 and add to queue
    level[starting_node] = 0;
    level_counts[0] = 1;
    queue[tail++] = starting_node;
    
    // BFS traversal
    LOOP_QUEUE: while (head != tail) {
        // Dequeue node
        node_index_t current_node = queue[head++];
        
        // Handle circular buffer wraparound
        if (head >= N_NODES) {
            head = 0;
        }
        
        // Get current node level
        level_t current_level = level[current_node];
        level_t next_level = current_level + 1;
        
        // Process all neighbors of current node
        edge_index_t edge_begin = nodes[current_node].edge_begin;
        edge_index_t edge_end = nodes[current_node].edge_end;
        
        LOOP_NEIGHBORS: for (edge_index_t i = edge_begin; i < edge_end; i++) {
            node_index_t neighbor = edges[i].dst;
            
            // If neighbor hasn't been visited yet
            if (level[neighbor] == MAX_LEVEL) {
                // Update level
                level[neighbor] = next_level;
                
                // Add to queue if within bounds
                if (next_level < N_LEVELS) {
                    level_counts[next_level]++;
                }
                
                // Enqueue neighbor
                queue[tail++] = neighbor;
                
                // Handle circular buffer wraparound
                if (tail >= N_NODES) {
                    tail = 0;
                }
            }
        }
    }
}
