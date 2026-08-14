
#include "bfs_queue.h"

void bfs(
    node_t nodes[N_NODES],
    edge_t edges[N_EDGES],
    node_index_t starting_node,
    level_t level[N_NODES],
    edge_index_t level_counts[N_LEVELS])
{
    // Initialize level array with MAX_LEVEL
    for (node_index_t i = 0; i < N_NODES; i++) {
        level[i] = MAX_LEVEL;
    }
    level[starting_node] = 0;

    // Initialize level_counts array with 0
    for (int i = 0; i < N_LEVELS; i++) {
        level_counts[i] = 0;
    }
    level_counts[0] = 1;

    // Circular queue implementation
    node_index_t queue[N_NODES];
    node_index_t queue_front = 0;
    node_index_t queue_back = 0;

    // Enqueue starting node
    queue[queue_back] = starting_node;
    queue_back = (queue_back + 1) % N_NODES;

    while (queue_front != queue_back) {
        // Dequeue current node
        node_index_t current_node = queue[queue_front];
        queue_front = (queue_front + 1) % N_NODES;

        level_t current_level = level[current_node];
        edge_index_t edge_begin = nodes[current_node].edge_begin;
        edge_index_t edge_end = nodes[current_node].edge_end;

        // Process all neighbors of current node
        for (edge_index_t i = edge_begin; i < edge_end; i++) {
            node_index_t neighbor = edges[i].dst;

            if (level[neighbor] == MAX_LEVEL) {
                level[neighbor] = current_level + 1;
                level_counts[current_level + 1]++;

                // Enqueue neighbor
                queue[queue_back] = neighbor;
                queue_back = (queue_back + 1) % N_NODES;
            }
        }
    }
}
