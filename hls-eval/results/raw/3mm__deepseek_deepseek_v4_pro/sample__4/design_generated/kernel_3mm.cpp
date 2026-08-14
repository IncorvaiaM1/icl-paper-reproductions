
#include "3mm.h"

void kernel_3mm(
    double E[16][18],
    double A[16][20],
    double B[20][18],
    double F[18][22],
    double C[18][24],
    double D[24][22],
    double G[16][22]) {
    int i, j, k;
    double sum;

    for (i = 0; i &lt; 16; i++) {
        for (j = 0; j &lt; 18; j++) {
            sum = 0.0;
            for (k = 0; k &lt; 20; k++) {
                sum += A[i][k] * B[k][j];
            }
            E[i][j] = sum;
        }
    }

    for (i = 0; i &lt; 18; i++) {
        for (j = 0; j &lt; 22; j++) {
            sum = 0.0;
            for (k = 0; k &lt; 24; k++) {
                sum += C[i][k] * D[k][j];
            }
            F[i][j] = sum;
        }
    }

    for (i = 0; i &lt; 16; i++) {
        for (j = 0; j &lt; 22; j++) {
            sum = 0.0;
            for (k = 0; k &lt; 18; k++) {
                sum += E[i][k] * F[k][j];
            }
            G[i][j] = sum;
        }
    }
}
