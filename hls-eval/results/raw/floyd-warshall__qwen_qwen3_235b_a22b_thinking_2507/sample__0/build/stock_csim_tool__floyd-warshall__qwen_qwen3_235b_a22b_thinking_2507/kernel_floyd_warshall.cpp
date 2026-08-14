
#include "floyd-warshall.h"

void kernel_floyd_warshall(int path[60][60]) {
    // Initialize diagonal to 0 (distance from node to itself is always 0)
    for (int i = 0; i < 60; i++) {
        path[i][i] = 0;
    }

    // Floyd-Warshall algorithm implementation
    // k: intermediate node
    // i: source node
    // j: destination node
    for (int k = 0; k < 60; k++) {
        for (int i = 0; i < 60; i++) {
            for (int j = 0; j < 60; j++) {
                // Calculate potential new path length through intermediate node k
                int new_distance = path[i][k] + path[k][j];
                
                // Update shortest path if new path is shorter
                if (new_distance < path[i][j]) {
                    path[i][j] = new_distance;
                }
            }
        }
    }
}
