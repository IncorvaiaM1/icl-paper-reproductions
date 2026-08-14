
#include "md_knn.h"
#include <math.h>

void md_kernel(
    TYPE force_x[nAtoms],
    TYPE force_y[nAtoms],
    TYPE force_z[nAtoms],
    TYPE position_x[nAtoms],
    TYPE position_y[nAtoms],
    TYPE position_z[nAtoms],
    int32_t NL[nAtoms * maxNeighbors]) {

    // Loop over each atom
    loop_i: for (int i = 0; i < nAtoms; i++) {
        TYPE fx = 0.0;
        TYPE fy = 0.0;
        TYPE fz = 0.0;

        TYPE ix = position_x[i];
        TYPE iy = position_y[i];
        TYPE iz = position_z[i];

        // Loop over each neighbor of the current atom
        loop_j: for (int j = 0; j < maxNeighbors; j++) {
            int neighbor_index = NL[i * maxNeighbors + j];

            // Check if the neighbor index is valid
            if (neighbor_index >= 0 && neighbor_index < nAtoms) {
                TYPE jx = position_x[neighbor_index];
                TYPE jy = position_y[neighbor_index];
                TYPE jz = position_z[neighbor_index];

                // Calculate the distance components
                TYPE dx = ix - jx;
                TYPE dy = iy - jy;
                TYPE dz = iz - jz;

                // Calculate the squared distance
                TYPE r2 = dx * dx + dy * dy + dz * dz;

                // Avoid division by zero
                if (r2 > 0.0) {
                    // Calculate r^2 and r^6
                    TYPE r2_inv = 1.0 / r2;
                    TYPE r6_inv = r2_inv * r2_inv * r2_inv;

                    // Calculate the force magnitude using Lennard-Jones potential
                    TYPE force_mag = (lj1 * r6_inv - lj2) * r6_inv * r2_inv;

                    // Update the force components
                    fx += force_mag * dx;
                    fy += force_mag * dy;
                    fz += force_mag * dz;
                }
            }
        }

        // Update the force arrays with the computed forces
        force_x[i] += fx;
        force_y[i] += fy;
        force_z[i] += fz;
    }
}
