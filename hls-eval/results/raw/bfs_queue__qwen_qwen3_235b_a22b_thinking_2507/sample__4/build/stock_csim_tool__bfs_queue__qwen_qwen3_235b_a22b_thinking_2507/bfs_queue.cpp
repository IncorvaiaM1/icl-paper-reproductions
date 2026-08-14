
#include "bfs_queue.h"

void bfs(node_t nodes[N_NODES], edge_t edges[N_EDGES], node_index_t starting_node, level_t level[N_NODES], edge_index_t level_counts[N_LEVELS]) {
    const int QUEUE_SIZE = N_NODES;
    node_index_t queue[QUEUE_SIZE];
    int head = 0;
    int tail = 0;

    // Initialize starting node level and level_counts[0]
    level[starting_node] = 0;
    level_counts[0] = 1;

    // Enqueue starting node
    queue[tail] = starting_node;
    tail = (tail + 1) % QUEUE_SIZE;

    // Process nodes in the queue until empty
    while (head != tail) {
        node_index_t current = queue[head];
        head = (head + 1) % QUEUE_SIZE;

        // Get edge range for current node
        edge_index_t begin = nodes[current].edge_begin;
        edge_index_t end = nodes[current].edge_end;

        // Traverse all neighbors
        for (edge_index_t i = begin; i < end; i++) {
            node_index_t neighbor = edges[i].dst;

            // Check if neighbor hasn't been visited
            if (level[neighbor] == MAX_LEVEL) {
                level_t next_level = level[current] + 1;
                level[neighbor] = next_level;

                // Enqueue neighbor
                queue[tail] = neighbor;
                tail = (tail + 1) % QUEUE_SIZE;

                // Update level counts if within bounds
                if (next_level < N_LEVELS) {
                    level_counts[next_level]++;
                }
            }
        }
    }
}
