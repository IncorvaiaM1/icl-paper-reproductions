
#include "floyd-warshall.h"

void kernel_floyd_warshall(int path[60][60]) {
    #pragma HLS INTERFACE m_axi port=path bundle=gmem
    #pragma HLS INTERFACE s_axilite port=path bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control

    int dist[60][60];
    #pragma HLS ARRAY_PARTITION variable=dist cyclic factor=4 dim=2

    // Initialize distance matrix
    for (int i = 0; i < 60; i++) {
        for (int j = 0; j < 60; j++) {
            #pragma HLS PIPELINE II=1
            dist[i][j] = path[i][j];
        }
    }

    // Floyd-Warshall algorithm
    for (int k = 0; k < 60; k++) {
        for (int i = 0; i < 60; i++) {
            for (int j = 0; j < 60; j++) {
                #pragma HLS PIPELINE II=1
                int new_dist = dist[i][k] + dist[k][j];
                if (dist[i][j] > new_dist) {
                    dist[i][j] = new_dist;
                }
            }
        }
    }

    // Copy back results
    for (int i = 0; i < 60; i++) {
        for (int j = 0; j < 60; j++) {
            #pragma HLS PIPELINE II=1
            path[i][j] = dist[i][j];
        }
    }
}
