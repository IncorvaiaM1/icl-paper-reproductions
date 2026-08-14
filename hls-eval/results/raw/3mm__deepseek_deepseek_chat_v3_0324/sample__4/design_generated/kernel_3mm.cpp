
#include "3mm.h"

void kernel_3mm(
    double E[16][18],
    double A[16][20],
    double B[20][18],
    double F[18][22],
    double C[18][24],
    double D[24][22],
    double G[16][22])
{
    // Compute E = A * B (16x20 * 20x18 = 16x18)
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 18; j++) {
            E[i][j] = 0;
            for (int k = 0; k < 20; k++) {
                E[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    // Compute F = C * D (18x24 * 24x22 = 18x22)
    for (int i = 0; i < 18; i++) {
        for (int j = 0; j < 22; j++) {
            F[i][j] = 0;
            for (int k = 0; k < 24; k++) {
                F[i][j] += C[i][k] * D[k][j];
            }
        }
    }

    // Compute G = E * F (16x18 * 18x22 = 16x22)
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 22; j++) {
            G[i][j] = 0;
            for (int k = 0; k < 18; k++) {
                G[i][j] += E[i][k] * F[k][j];
            }
        }
    }
}
