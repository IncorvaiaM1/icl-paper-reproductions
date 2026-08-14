
#include "covariance.h"

void kernel_covariance(
    double float_n,
    double data[32][28],
    double cov[28][28],
    double mean[28])
{
    int i, j, k;
    double centered_data[32][28];

    // Step 1: Compute the mean for each attribute
    for (j = 0; j < 28; j++) {
        double sum = 0.0;
        for (k = 0; k < 32; k++) {
            sum += data[k][j];
        }
        mean[j] = sum / float_n;
    }

    // Step 2: Center the data by subtracting the mean
    for (k = 0; k < 32; k++) {
        for (j = 0; j < 28; j++) {
            centered_data[k][j] = data[k][j] - mean[j];
        }
    }

    // Step 3: Compute the covariance matrix
    for (i = 0; i < 28; i++) {
        for (j = 0; j < 28; j++) {
            double sum = 0.0;
            for (k = 0; k < 32; k++) {
                sum += centered_data[k][i] * centered_data[k][j];
            }
            cov[i][j] = sum / (float_n - 1.0);
        }
    }
}
