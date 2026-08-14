
#include "fdtd-2d.h"

void kernel_fdtd_2d(
    double ex[20][30],
    double ey[20][30],
    double hz[20][30],
    double _fict_[20])
{
    // Constants (simplified as scalar coefficients as mentioned in description)
    const double Chzh = 0.7;
    const double Chze = 0.8;
    const double Cexe = 0.9;
    const double Cexh = 0.1;
    const double Ceye = 0.9;
    const double Ceyh = 0.1;

    // Temporary arrays to store intermediate results
    double hz_new[20][30];
    double ex_new[20][30];
    double ey_new[20][30];

    int t, i, j;

    // Time steps (tmax is 20 as per testbench)
    for (t = 0; t < 20; t++) {
        // Update Hz field
        for (i = 1; i < 19; i++) {
            for (j = 1; j < 29; j++) {
                hz_new[i][j] = Chzh * hz[i][j] + 
                               Chze * (ex[i][j+1] - ex[i][j] - ey[i+1][j] + ey[i][j]);
            }
        }

        // Apply boundary conditions for Hz (from fictitious source)
        for (i = 0; i < 20; i++) {
            hz_new[i][0] = _fict_[t];
            hz_new[i][29] = _fict_[t];
        }
        for (j = 0; j < 30; j++) {
            hz_new[0][j] = _fict_[t];
            hz_new[19][j] = _fict_[t];
        }

        // Update Ex field
        for (i = 0; i < 20; i++) {
            for (j = 1; j < 29; j++) {
                ex_new[i][j] = Cexe * ex[i][j] + 
                               Cexh * (hz_new[i][j] - hz_new[i][j-1]);
            }
        }

        // Update Ey field
        for (i = 1; i < 19; i++) {
            for (j = 0; j < 30; j++) {
                ey_new[i][j] = Ceye * ey[i][j] + 
                               Ceyh * (hz_new[i][j] - hz_new[i-1][j]);
            }
        }

        // Copy new values back to original arrays
        for (i = 0; i < 20; i++) {
            for (j = 0; j < 30; j++) {
                hz[i][j] = hz_new[i][j];
                if (j > 0 && j < 29) ex[i][j] = ex_new[i][j];
                if (i > 0 && i < 19) ey[i][j] = ey_new[i][j];
            }
        }
    }
}
