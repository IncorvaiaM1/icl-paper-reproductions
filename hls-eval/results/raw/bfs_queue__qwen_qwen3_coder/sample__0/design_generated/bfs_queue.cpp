
#include "bfs_queue.h"

void init_levels(level_t level[N_NODES]) {
#pragma HLS INLINE
    for (int i = 0; i < N_NODES; i++) {
#pragma HLS PIPELINE II=1
        level[i] = MAX_LEVEL;
    }
}

void init_horizons(edge_index_t level_counts[N_LEVELS]) {
#pragma HLS INLINE
    for (int i = 0; i < N_LEVELS; i++) {
#pragma HLS PIPELINE II=1
        level_counts[i] = 0;
    }
}

void bfs(
    node_t nodes[N_NODES],
    edge_t edges[N_EDGES],
    node_index_t starting_node,
    level_t level[N_NODES],
    edge_index_t level_counts[N_LEVELS]) {

    // Local queue implemented as circular buffer
    node_index_t queue[N_NODES];
#pragma HLS RESOURCE variable=queue core=RAM_2P
    node_index_t head = 0;
    node_index_t tail = 0;
    node_index_t size = 0;

    // Initialize level array and level_counts array
    init_levels(level);
    init_horizons(level_counts);

    // Set starting node level to 0 and add to queue
    level[starting_node] = 0;
    queue[tail] = starting_node;
    tail = (tail + 1) % N_NODES;
    size++;
    level_counts[0] = 1;

    // Current level being processed
    level_t current_level = 0;

    // BFS traversal
    LOOP_QUEUE:
    while (size > 0) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=N_NODES
        // Process all nodes at the current level
        edge_index_t nodes_at_current_level = level_counts[current_level];
        edge_index_t nodes_processed = 0;

        LOOP_CURRENT_LEVEL:
        while (nodes_processed < nodes_at_current_level && size > 0) {
#pragma HLS LOOP_TRIPCOUNT min=1 max=N_NODES
            // Dequeue a node
            node_index_t node = queue[head];
            head = (head + 1) % N_NODES;
            size--;

            // Get the range of edges for the current node
            edge_index_t edge_begin = nodes[node].edge_begin;
            edge_index_t edge_end = nodes[node].edge_end;

            // Traverse all neighbors
            LOOP_NEIGHBORS:
            for (edge_index_t i = edge_begin; i < edge_end; i++) {
#pragma HLS LOOP_TRIPCOUNT min=0 max=EDGE_FACTOR
                node_index_t neighbor = edges[i].dst;

                // If the neighbor hasn't been visited yet
                if (level[neighbor] == MAX_LEVEL) {
                    level[neighbor] = current_level + 1;
                    
                    // Enqueue the neighbor
                    queue[tail] = neighbor;
                    tail = (tail + 1) % N_NODES;
                    size++;

                    // Update level_counts for the next level
                    if (current_level + 1 < N_LEVELS) {
                        level_counts[current_level + 1]++;
                    }
                }
            }
            nodes_processed++;
        }
        current_level++;
        
        // Exit if we've exceeded the maximum level count
        if (current_level >= N_LEVELS) {
            break;
        }
    }
}
