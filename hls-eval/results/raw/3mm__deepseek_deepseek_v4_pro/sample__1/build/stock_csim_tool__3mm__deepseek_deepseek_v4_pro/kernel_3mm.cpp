
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
    int i, j, k;

    // E = A * B
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 18; j++) {
            double sum = 0.0;
            for (k = 0; k < 20; k++) {
                sum += A[i][k] * B[k][j];
            }
            E[i][j] = sum;
        }
    }

    // F = C * D
    for (i = 0; i < 18; i++) {
        for (j = 0; j < 22; j++) {
            double sum = 0.0;
            for (k = 0; k < 24; k++) {
                sum += C[i][k] * D[k][j];
            }
            F[i][j] = sum;
        }
    }

    // G = E * F
    for (i = 0; i < 16; i++) {
        for (j = 0; j < 22; j++) {
            double sum = 0.0;
            for (k = 0; k < 18; k++) {
                sum += E[i][k] * F[k][j];
            }
            G[i][j] = sum;
        }
    }
}
