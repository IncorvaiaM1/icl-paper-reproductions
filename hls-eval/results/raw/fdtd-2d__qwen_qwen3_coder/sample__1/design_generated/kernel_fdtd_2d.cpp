
#include "fdtd-2d.h"

void kernel_fdtd_2d(
    double ex[20][30],
    double ey[20][30],
    double hz[20][30],
    double _fict_[20]) {

    // Coefficients as defined in PolyBench
    const double cz = 0.5; // Example value; in real scenario, these may vary per location

    const double chx = 0.8;
    const double chy = 0.9;

    const double cexe = 0.1;
    const double cexh = 0.2;

    const double ceye = 0.3;
    const double ceyh = 0.4;

    const double chzh = 0.5;
    const double chze = 0.6;

    int t, i, j;

    // Time iteration loop
    for (t = 0; t < 20; t++) {
        // Update Hz field
        for (i = 0; i < 20 - 1; i++) {
            for (j = 0; j < 30 - 1; j++) {
                hz[i][j] = chzh * hz[i][j] +
                           chze * (ex[i][j + 1] - ex[i][j] - ey[i + 1][j] + ey[i][j]);
            }
        }

        // Apply boundary condition using fictitious source
        for (i = 0; i < 20; i++) {
            ey[i][0] = _fict_[t];
        }

        // Update Ex field
        for (i = 0; i < 20; i++) {
            for (j = 1; j < 30; j++) {
                ex[i][j] = cexe * ex[i][j] +
                           cexh * (hz[i][j] - hz[i][j - 1]);
            }
        }

        // Update Ey field
        for (i = 1; i < 20; i++) {
            for (j = 0; j < 30; j++) {
                ey[i][j] = ceye * ey[i][j] +
                           ceyh * (hz[i][j] - hz[i - 1][j]);
            }
        }
    }
}
