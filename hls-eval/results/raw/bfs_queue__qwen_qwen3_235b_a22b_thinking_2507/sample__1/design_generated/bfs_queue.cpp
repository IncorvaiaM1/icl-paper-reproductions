
#include "bfs_queue.h"

void bfs(node_t nodes[N_NODES], edge_t edges[N_EDGES], node_index_t starting_node, level_t level[N_NODES], edge_index_t level_counts[N_LEVELS]) {
    const int QUEUE_SIZE = N_NODES + 1;
    node_index_t queue[QUEUE_SIZE];
    int head = 0;
    int tail = 0;

    // Initialize starting node
    level[starting_node] = 0;
    level_counts[0] = 1;

    // Enqueue starting node
    queue[tail] = starting_node;
    tail = (tail + 1) % QUEUE_SIZE;

    // Process nodes until queue is empty
    while (head != tail) {
        // Dequeue node
        node_index_t u = queue[head];
        head = (head + 1) % QUEUE_SIZE;
        level_t current_level = level[u];

        // Process all neighbors of node u
        for (edge_index_t i = nodes[u].edge_begin; i < nodes[u].edge_end; i++) {
            node_index_t v = edges[i].dst;
            
            // If neighbor hasn't been visited
            if (level[v] == MAX_LEVEL) {
                // Update level and level counts
                level[v] = current_level + 1;
                if (current_level + 1 < N_LEVELS) {
                    level_counts[current_level + 1]++;
                }
                
                // Enqueue newly discovered node
                queue[tail] = v;
                tail = (tail + 1) % QUEUE_SIZE;
            }
        }
    }
}
