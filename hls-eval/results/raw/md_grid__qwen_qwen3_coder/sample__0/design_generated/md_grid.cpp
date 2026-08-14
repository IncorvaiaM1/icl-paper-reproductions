
#include "md_grid.h"

void md(
    int32_t n_points[blockSide][blockSide][blockSide],
    dvector_t force[blockSide][blockSide][blockSide][densityFactor],
    dvector_t position[blockSide][blockSide][blockSide][densityFactor]) {

    // Iterate over the 3D grid of blocks
    loop_grid0:
    for (int bx = 0; bx < blockSide; bx++) {
        for (int by = 0; by < blockSide; by++) {
            for (int bz = 0; bz < blockSide; bz++) {
                // Iterate over the 3x3x3 cube of blocks around a given block
                loop_grid1:
                for (int dx = -1; dx <= 1; dx++) {
                    for (int dy = -1; dy <= 1; dy++) {
                        for (int dz = -1; dz <= 1; dz++) {
                            // Calculate neighbor block indices with periodic boundary conditions
                            int nx = (bx + dx + blockSide) % blockSide;
                            int ny = (by + dy + blockSide) % blockSide;
                            int nz = (bz + dz + blockSide) % blockSide;

                            // Get number of points in the neighbor block
                            int nq = n_points[nx][ny][nz];

                            // Iterate over the points in the current block
                            loop_p:
                            for (int p = 0; p < n_points[bx][by][bz]; p++) {
                                // Get position of point p in the current block
                                dvector_t pos_p = position[bx][by][bz][p];

                                // Initialize force accumulator for point p
                                dvector_t force_acc = {0.0, 0.0, 0.0};

                                // Iterate over the points in the neighbor block
                                loop_q:
                                for (int q = 0; q < nq; q++) {
                                    // Get position of point q in the neighbor block
                                    dvector_t pos_q = position[nx][ny][nz][q];

                                    // Calculate distance vector
                                    dvector_t dr;
                                    dr.x = pos_p.x - pos_q.x;
                                    dr.y = pos_p.y - pos_q.y;
                                    dr.z = pos_p.z - pos_q.z;

                                    // Calculate squared distance
                                    TYPE r2 = dr.x * dr.x + dr.y * dr.y + dr.z * dr.z;

                                    // Skip if points are too close (avoid division by zero)
                                    if (r2 < 1e-12) continue;

                                    // Calculate inverse distance powers
                                    TYPE r2_inv = 1.0 / r2;
                                    TYPE r6_inv = r2_inv * r2_inv * r2_inv;
                                    TYPE r8_inv = r6_inv * r2_inv;

                                    // Calculate Lennard-Jones force magnitude
                                    TYPE lj_force = (6.0 * lj1 * r6_inv - 6.0 * lj2) * r8_inv;

                                    // Update force accumulator
                                    force_acc.x += lj_force * dr.x;
                                    force_acc.y += lj_force * dr.y;
                                    force_acc.z += lj_force * dr.z;
                                }

                                // Update force for point p
                                force[bx][by][bz][p].x += force_acc.x;
                                force[bx][by][bz][p].y += force_acc.y;
                                force[bx][by][bz][p].z += force_acc.z;
                            }
                        }
                    }
                }
            }
        }
    }
}
