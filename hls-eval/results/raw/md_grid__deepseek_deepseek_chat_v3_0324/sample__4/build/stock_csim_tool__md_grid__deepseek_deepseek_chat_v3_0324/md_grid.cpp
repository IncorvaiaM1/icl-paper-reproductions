
#include "md_grid.h"

void md(
    int32_t n_points[blockSide][blockSide][blockSide],
    dvector_t force[blockSide][blockSide][blockSide][densityFactor],
    dvector_t position[blockSide][blockSide][blockSide][densityFactor]) {

    // Temporary force array to accumulate forces
    dvector_t temp_force[blockSide][blockSide][blockSide][densityFactor];
    #pragma HLS ARRAY_PARTITION variable=temp_force complete dim=1
    #pragma HLS ARRAY_PARTITION variable=temp_force complete dim=2
    #pragma HLS ARRAY_PARTITION variable=temp_force complete dim=3

    // Initialize temporary forces to zero
    for (int i = 0; i < blockSide; i++) {
        for (int j = 0; j < blockSide; j++) {
            for (int k = 0; k < blockSide; k++) {
                for (int l = 0; l < densityFactor; l++) {
                    temp_force[i][j][k][l].x = 0;
                    temp_force[i][j][k][l].y = 0;
                    temp_force[i][j][k][l].z = 0;
                }
            }
        }
    }

    // Main computation loops
    for (int i = 0; i < blockSide; i++) {
        for (int j = 0; j < blockSide; j++) {
            for (int k = 0; k < blockSide; k++) {
                // Loop over neighboring blocks (3x3x3 cube centered at current block)
                for (int di = -1; di <= 1; di++) {
                    for (int dj = -1; dj <= 1; dj++) {
                        for (int dk = -1; dk <= 1; dk++) {
                            int ni = i + di;
                            int nj = j + dj;
                            int nk = k + dk;

                            // Check if neighbor block is within bounds
                            if (ni >= 0 && ni < blockSide &&
                                nj >= 0 && nj < blockSide &&
                                nk >= 0 && nk < blockSide) {

                                // Loop over points in current block
                                for (int p = 0; p < n_points[i][j][k]; p++) {
                                    // Loop over points in neighbor block
                                    for (int q = 0; q < n_points[ni][nj][nk]; q++) {
                                        // Skip self-interaction
                                        if (i == ni && j == nj && k == nk && p == q) {
                                            continue;
                                        }

                                        // Calculate distance components
                                        TYPE dx = position[i][j][k][p].x - position[ni][nj][nk][q].x;
                                        TYPE dy = position[i][j][k][p].y - position[ni][nj][nk][q].y;
                                        TYPE dz = position[i][j][k][p].z - position[ni][nj][nk][q].z;

                                        // Calculate squared distance
                                        TYPE r2 = dx*dx + dy*dy + dz*dz;

                                        // Prevent division by zero
                                        if (r2 == 0) continue;

                                        // Calculate r^-6
                                        TYPE r6 = 1.0 / (r2 * r2 * r2);

                                        // Calculate Lennard-Jones potential terms
                                        TYPE potential = r6 * (lj1 * r6 - lj2);

                                        // Calculate force magnitude (6*r^-7*(lj1*r^-6 - lj2) + 6*lj1*r^-13)
                                        TYPE force_magnitude = 6.0 * potential / r2 + 6.0 * lj1 * r6 * r6 / r2;

                                        // Calculate force components
                                        temp_force[i][j][k][p].x += force_magnitude * dx;
                                        temp_force[i][j][k][p].y += force_magnitude * dy;
                                        temp_force[i][j][k][p].z += force_magnitude * dz;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Update the output force array
    for (int i = 0; i < blockSide; i++) {
        for (int j = 0; j < blockSide; j++) {
            for (int k = 0; k < blockSide; k++) {
                for (int l = 0; l < n_points[i][j][k]; l++) {
                    force[i][j][k][l].x = temp_force[i][j][k][l].x;
                    force[i][j][k][l].y = temp_force[i][j][k][l].y;
                    force[i][j][k][l].z = temp_force[i][j][k][l].z;
                }
            }
        }
    }
}
