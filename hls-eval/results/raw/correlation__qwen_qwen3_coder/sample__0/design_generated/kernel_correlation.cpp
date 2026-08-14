
#include "correlation.h"

void kernel_correlation(
    double float_n,
    double data[32][28],
    double corr[28][28],
    double mean[28],
    double stddev[28]) {

    int i, j, k;

    // Step 1: Compute the mean of each attribute
    for (j = 0; j < 28; j++) {
        mean[j] = 0.0;
        for (i = 0; i < 32; i++) {
            mean[j] += data[i][j];
        }
        mean[j] /= float_n;
    }

    // Step 2: Compute the standard deviation of each attribute
    for (j = 0; j < 28; j++) {
        stddev[j] = 0.0;
        for (i = 0; i < 32; i++) {
            double diff = data[i][j] - mean[j];
            stddev[j] += diff * diff;
        }
        stddev[j] /= float_n;
        stddev[j] = sqrt(stddev[j]);
    }

    // Step 3: Normalize the data
    double data_norm[32][28];
    for (i = 0; i < 32; i++) {
        for (j = 0; j < 28; j++) {
            if (stddev[j] != 0) {
                data_norm[i][j] = (data[i][j] - mean[j]) / stddev[j];
            } else {
                data_norm[i][j] = 0.0;
            }
        }
    }

    // Step 4: Compute the correlation matrix
    for (i = 0; i < 28; i++) {
        for (j = 0; j < 28; j++) {
            if (i <= j) {
                double sum = 0.0;
                for (k = 0; k < 32; k++) {
                    sum += data_norm[k][i] * data_norm[k][j];
                }
                corr[i][j] = sum / float_n;
                corr[j][i] = corr[i][j]; // Symmetric matrix
            }
        }
    }
}
