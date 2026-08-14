
#include "md_grid.h"

void md(
    int32_t n_points[blockSide][blockSide][blockSide],
    dvector_t force[blockSide][blockSide][blockSide][densityFactor],
    dvector_t position[blockSide][blockSide][blockSide][densityFactor]) {
    
    // Iterate over each block in the 3D grid (loop_grid0)
    for (int i = 0; i < blockSide; i++) {
        for (int j = 0; j < blockSide; j++) {
            for (int k = 0; k < blockSide; k++) {
                
                // Iterate over the 3x3x3 neighborhood of blocks (loop_grid1)
                for (int di = -1; di <= 1; di++) {
                    for (int dj = -1; dj <= 1; dj++) {
                        for (int dk = -1; dk <= 1; dk++) {
                            
                            int ii = i + di;
                            int jj = j + dj;
                            int kk = k + dk;
                            
                            // Check if neighbor block is within grid boundaries
                            if (ii >= 0 && ii < blockSide && 
                                jj >= 0 && jj < blockSide && 
                                kk >= 0 && kk < blockSide) {
                                
                                // Iterate over points in current block (loop_p)
                                for (int p = 0; p < n_points[i][j][k]; p++) {
                                    
                                    // Iterate over points in neighbor block (loop_q)
                                    for (int q = 0; q < n_points[ii][jj][kk]; q++) {
                                        
                                        // Skip self-interaction (same point in same block)
                                        if (i == ii && j == jj && k == kk && p == q) {
                                            continue;
                                        }
                                        
                                        // Calculate position differences
                                        TYPE dx = position[i][j][k][p].x - position[ii][jj][kk][q].x;
                                        TYPE dy = position[i][j][k][p].y - position[ii][jj][kk][q].y;
                                        TYPE dz = position[i][j][k][p].z - position[ii][jj][kk][q].z;
                                        
                                        // Calculate squared distance
                                        TYPE r2 = dx*dx + dy*dy + dz*dz;
                                        
                                        // Avoid division by zero (though self-interaction is skipped)
                                        if (r2 == 0) {
                                            continue;
                                        }
                                        
                                        // Calculate inverse distance terms
                                        TYPE inv_r2 = 1.0 / r2;
                                        TYPE inv_r6 = inv_r2 * inv_r2 * inv_r2;
                                        TYPE inv_r12 = inv_r6 * inv_r6;
                                        
                                        // Calculate force scalar factor using LJ parameters
                                        TYPE force_scalar = (12.0 * lj1 * inv_r12 - 6.0 * lj2 * inv_r6) * inv_r2;
                                        
                                        // Update force on current point (center block)
                                        force[i][j][k][p].x += dx * force_scalar;
                                        force[i][j][k][p].y += dy * force_scalar;
                                        force[i][j][k][p].z += dz * force_scalar;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
