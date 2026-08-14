
#include "heat-3d.h"

void kernel_heat_3d(double A[10][10][10], double B[10][10][10]) {
    // Perform 20 time steps of heat equation simulation
    for (int t = 0; t < 20; t++) {
        if (t % 2 == 0) {
            // Update B from A for interior points (1-8 in each dimension)
            for (int i = 1; i < 9; i++) {
                for (int j = 1; j < 9; j++) {
                    for (int k = 1; k < 9; k++) {
                        B[i][j][k] = A[i][j][k] 
                                    + 0.125 * (A[i+1][j][k] - 2.0 * A[i][j][k] + A[i-1][j][k])
                                    + 0.125 * (A[i][j+1][k] - 2.0 * A[i][j][k] + A[i][j-1][k])
                                    + 0.125 * (A[i][j][k+1] - 2.0 * A[i][j][k] + A[i][j][k-1]);
                    }
                }
            }
        } else {
            // Update A from B for interior points (1-8 in each dimension)
            for (int i = 1; i < 9; i++) {
                for (int j = 1; j < 9; j++) {
                    for (int k = 1; k < 9; k++) {
                        A[i][j][k] = B[i][j][k] 
                                    + 0.125 * (B[i+1][j][k] - 2.0 * B[i][j][k] + B[i-1][j][k])
                                    + 0.125 * (B[i][j+1][k] - 2.0 * B[i][j][k] + B[i][j-1][k])
                                    + 0.125 * (B[i][j][k+1] - 2.0 * B[i][j][k] + B[i][j][k-1]);
                    }
                }
            }
        }
    }
    
    // After 20 steps, result is in A (since 20 is even), but output must be in B
    // Copy entire grid from A to B
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            for (int k = 0; k < 10; k++) {
                B[i][j][k] = A[i][j][k];
            }
        }
    }
}
