
#include "floyd-warshall.h"

void kernel_floyd_warshall(int path[60][60]) {
    int k, i, j;

    for (k = 0; k < 60; k++) {
        for (i = 0; i < 60; i++) {
            for (j = 0; j < 60; j++) {
                int temp = path[i][k] + path[k][j];
                if (temp < path[i][j]) {
                    path[i][j] = temp;
                }
            }
        }
    }
}
