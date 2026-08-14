
#include "fdtd-2d.h"

void kernel_fdtd_2d(
    double ex[20][30],
    double ey[20][30],
    double hz[20][30],
    double _fict_[20]) {

    // Define coefficients (simplified as constants as per PolyBench)
    const double C_hzh = 0.5;
    const double C_hze = 0.5;
    const double C_exe = 0.5;
    const double C_exh = 0.5;
    const double C_eye = 0.5;
    const double C_eyh = 0.5;

    // Temporary arrays to store updated values
    double ex_new[20][30];
    double ey_new[20][30];
    double hz_new[20][30];

    // Initialize temporary arrays
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 30; j++) {
            ex_new[i][j] = ex[i][j];
            ey_new[i][j] = ey[i][j];
            hz_new[i][j] = hz[i][j];
        }
    }

    // Main computation loop
    for (int t = 0; t < 20; t++) {
        // Update Hz field
        for (int i = 1; i < 20 - 1; i++) {
            for (int j = 1; j < 30 - 1; j++) {
                hz_new[i][j] = C_hzh * hz[i][j] + 
                              C_hze * (ex[i][j+1] - ex[i][j] - ey[i+1][j] + ey[i][j]);
            }
        }

        // Apply boundary condition for Hz
        for (int i = 0; i < 20; i++) {
            hz_new[i][0] = _fict_[t];
            hz_new[i][30-1] = _fict_[t];
        }
        for (int j = 0; j < 30; j++) {
            hz_new[0][j] = _fict_[t];
            hz_new[20-1][j] = _fict_[t];
        }

        // Update Ex field
        for (int i = 0; i < 20; i++) {
            for (int j = 1; j < 30; j++) {
                ex_new[i][j] = C_exe * ex[i][j] + 
                              C_exh * (hz_new[i][j] - hz_new[i][j-1]);
            }
        }

        // Update Ey field
        for (int i = 1; i < 20; i++) {
            for (int j = 0; j < 30; j++) {
                ey_new[i][j] = C_eye * ey[i][j] + 
                              C_eyh * (hz_new[i][j] - hz_new[i-1][j]);
            }
        }

        // Copy new values back to original arrays for next iteration
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 30; j++) {
                hz[i][j] = hz_new[i][j];
                ex[i][j] = ex_new[i][j];
                ey[i][j] = ey_new[i][j];
            }
        }
    }
}
