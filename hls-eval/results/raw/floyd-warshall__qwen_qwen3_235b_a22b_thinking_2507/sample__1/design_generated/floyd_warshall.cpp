
#include "floyd-warshall.h"

void kernel_floyd_warshall(int path[60][60]) {
    // Initialize diagonal to 0 (distance from node to itself is 0)
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
                // Update shortest path if a shorter path through node k is found
                // Note: If path[i][k] or path[k][j] is infinity (999), the sum will be >=999
                // and won't update path[i][j] if it's already <=999
                if (path[i][k] + path[k][j] < path[i][j]) {
                    path[i][j] = path[i][k] + path[k][j];
                }
            }
        }
    }
}
