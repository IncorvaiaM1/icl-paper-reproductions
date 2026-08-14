
#include "md_knn.h"

void md_kernel(
    TYPE force_x[nAtoms],
    TYPE force_y[nAtoms],
    TYPE force_z[nAtoms],
    TYPE position_x[nAtoms],
    TYPE position_y[nAtoms],
    TYPE position_z[nAtoms],
    int32_t NL[nAtoms * maxNeighbors]) {

    // Iterate over each atom
    loop_i:
    for (int i = 0; i < nAtoms; i++) {
        TYPE fx = 0.0;
        TYPE fy = 0.0;
        TYPE fz = 0.0;

        // Iterate over each neighbor of the current atom
        loop_j:
        for (int j = 0; j < maxNeighbors; j++) {
            int neighbor_index = NL[i * maxNeighbors + j];

            // Check if the neighbor index is valid (not -1)
            if (neighbor_index != -1) {
                // Calculate the distance vector between atom i and its neighbor
                TYPE dx = position_x[neighbor_index] - position_x[i];
                TYPE dy = position_y[neighbor_index] - position_y[i];
                TYPE dz = position_z[neighbor_index] - position_z[i];

                // Calculate the squared distance
                TYPE r2 = dx * dx + dy * dy + dz * dz;

                // Avoid division by zero
                if (r2 > 0.0) {
                    // Calculate r2_inv and r6_inv
                    TYPE r2_inv = 1.0 / r2;
                    TYPE r6_inv = r2_inv * r2_inv * r2_inv;

                    // Calculate the force magnitude using Lennard-Jones potential
                    TYPE force_mag = lj1 * r6_inv - lj2 * r6_inv * r6_inv;

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
