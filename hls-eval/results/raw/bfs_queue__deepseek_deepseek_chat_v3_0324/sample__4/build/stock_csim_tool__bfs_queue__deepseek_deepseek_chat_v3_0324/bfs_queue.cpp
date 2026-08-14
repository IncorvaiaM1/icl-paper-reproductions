
#include "bfs_queue.h"

void bfs(
    node_t nodes[N_NODES],
    edge_t edges[N_EDGES],
    node_index_t starting_node,
    level_t level[N_NODES],
    edge_index_t level_counts[N_LEVELS]) {

    // Initialize levels
    for (node_index_t i = 0; i < N_NODES; i++) {
        level[i] = MAX_LEVEL;
    }
    level[starting_node] = 0;

    // Initialize level counts
    for (int i = 0; i < N_LEVELS; i++) {
        level_counts[i] = 0;
    }
    level_counts[0] = 1;

    // Circular queue implementation
    node_index_t queue[N_NODES];
    node_index_t queue_front = 0;
    node_index_t queue_rear = 0;

    // Enqueue starting node
    queue[queue_rear] = starting_node;
    queue_rear = (queue_rear + 1) % N_NODES;

    while (queue_front != queue_rear) {
        // Dequeue current node
        node_index_t current_node = queue[queue_front];
        queue_front = (queue_front + 1) % N_NODES;

        level_t current_level = level[current_node];

        // Process all neighbors
        for (edge_index_t edge_idx = nodes[current_node].edge_begin;
             edge_idx < nodes[current_node].edge_end;
             edge_idx++) {
            node_index_t neighbor = edges[edge_idx].dst;

            // If neighbor hasn't been visited yet
            if (level[neighbor] == MAX_LEVEL) {
                level[neighbor] = current_level + 1;
                
                // Update level counts
                if (current_level + 1 < N_LEVELS) {
                    level_counts[current_level + 1]++;
                }

                // Enqueue neighbor
                queue[queue_rear] = neighbor;
                queue_rear = (queue_rear + 1) % N_NODES;
            }
        }
    }
}
