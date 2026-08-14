
#include "2mm.h"

void kernel_2mm(double alpha, double beta, double tmp[16][18], double A[16][22], double B[22][18], double C[18][24], double D[16][24]) {
    // Compute tmp = A * B
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 18; j++) {
            tmp[i][j] = 0.0;
            for (int k = 0; k < 22; k++) {
                tmp[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    // Compute tmp * C and scale by alpha, then accumulate with beta*D
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 24; j++) {
            double acc = 0.0;
            for (int k = 0; k < 18; k++) {
                acc += tmp[i][k] * C[k][j];
            }
            D[i][j] = alpha * acc + beta * D[i][j];
        }
    }
}
