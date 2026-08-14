
#include "jacobi-2d.h"

void kernel_jacobi_2d(double A[30][30], double B[30][30]) {
    const int size = 30;
    const int time_steps = 20;

    // Ping-pong buffers
    double (*src)[30][30] = &A;
    double (*dst)[30][30] = &B;

    for (int t = 0; t < time_steps; t++) {
        // Process each element in the 2D grid
        for (int i = 1; i < size - 1; i++) {
            for (int j = 1; j < size - 1; j++) {
                // Perform 5-point stencil computation
                (*dst)[i][j] = ((*src)[i][j] + (*src)[i-1][j] + (*src)[i+1][j] +
                               (*src)[i][j-1] + (*src)[i][j+1]) * 0.2;
            }
        }

        // Handle boundary conditions (copy from source to destination)
        for (int i = 0; i < size; i++) {
            (*dst)[i][0] = (*src)[i][0];
            (*dst)[i][size-1] = (*src)[i][size-1];
        }
        for (int j = 0; j < size; j++) {
            (*dst)[0][j] = (*src)[0][j];
            (*dst)[size-1][j] = (*src)[size-1][j];
        }

        // Swap source and destination buffers for next iteration
        double (*temp)[30][30] = src;
        src = dst;
        dst = temp;
    }

    // Final result might be in either A or B depending on number of iterations
    // If odd number of iterations, we need to copy back to A
    if (time_steps % 2 == 1) {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                A[i][j] = B[i][j];
            }
        }
    }
}
