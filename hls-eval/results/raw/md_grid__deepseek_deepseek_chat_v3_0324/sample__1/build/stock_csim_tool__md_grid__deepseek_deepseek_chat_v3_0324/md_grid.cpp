
#include "md_grid.h"

void md(
    int32_t n_points[blockSide][blockSide][blockSide],
    dvector_t force[blockSide][blockSide][blockSide][densityFactor],
    dvector_t position[blockSide][blockSide][blockSide][densityFactor]) {

    // Temporary force accumulator for each point
    dvector_t force_local[blockSide][blockSide][blockSide][densityFactor];
    
    // Initialize local forces to zero
    for (int i = 0; i < blockSide; i++) {
        for (int j = 0; j < blockSide; j++) {
            for (int k = 0; k < blockSide; k++) {
                for (int p = 0; p < densityFactor; p++) {
                    force_local[i][j][k][p].x = 0.0;
                    force_local[i][j][k][p].y = 0.0;
                    force_local[i][j][k][p].z = 0.0;
                }
            }
        }
    }

    // Main computation loop - iterate over all blocks
    for (int i = 0; i < blockSide; i++) {
        for (int j = 0; j < blockSide; j++) {
            for (int k = 0; k < blockSide; k++) {
                int np_i = n_points[i][j][k];
                
                // Iterate over neighboring blocks (3x3x3 cube)
                for (int di = -1; di <= 1; di++) {
                    for (int dj = -1; dj <= 1; dj++) {
                        for (int dk = -1; dk <= 1; dk++) {
                            int ni = i + di;
                            int nj = j + dj;
                            int nk = k + dk;
                            
                            // Check if neighbor is within bounds
                            if (ni >= 0 && ni < blockSide && 
                                nj >= 0 && nj < blockSide && 
                                nk >= 0 && nk < blockSide) {
                                
                                int np_j = n_points[ni][nj][nk];
                                
                                // Iterate over all points in current block
                                for (int p = 0; p < np_i; p++) {
                                    // Iterate over all points in neighbor block
                                    for (int q = 0; q < np_j; q++) {
                                        // Skip self-interaction
                                        if (i == ni && j == nj && k == nk && p == q) {
                                            continue;
                                        }
                                        
                                        // Calculate distance between points
                                        TYPE dx = position[i][j][k][p].x - position[ni][nj][nk][q].x;
                                        TYPE dy = position[i][j][k][p].y - position[ni][nj][nk][q].y;
                                        TYPE dz = position[i][j][k][p].z - position[ni][nj][nk][q].z;
                                        
                                        // Calculate squared distance
                                        TYPE r2 = dx*dx + dy*dy + dz*dz;
                                        
                                        // Avoid division by zero
                                        if (r2 == 0.0) continue;
                                        
                                        // Calculate r^-6 and r^-12
                                        TYPE r2inv = 1.0 / r2;
                                        TYPE r6inv = r2inv * r2inv * r2inv;
                                        TYPE r12inv = r6inv * r6inv;
                                        
                                        // Lennard-Jones potential derivative terms
                                        TYPE f = (12.0 * lj1 * r12inv - 6.0 * lj2 * r6inv) * r2inv;
                                        
                                        // Update force components
                                        force_local[i][j][k][p].x += f * dx;
                                        force_local[i][j][k][p].y += f * dy;
                                        force_local[i][j][k][p].z += f * dz;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Add the computed forces to the output array
    for (int i = 0; i < blockSide; i++) {
        for (int j = 0; j < blockSide; j++) {
            for (int k = 0; k < blockSide; k++) {
                int np = n_points[i][j][k];
                for (int p = 0; p < np; p++) {
                    force[i][j][k][p].x += force_local[i][j][k][p].x;
                    force[i][j][k][p].y += force_local[i][j][k][p].y;
                    force[i][j][k][p].z += force_local[i][j][k][p].z;
                }
            }
        }
    }
}
