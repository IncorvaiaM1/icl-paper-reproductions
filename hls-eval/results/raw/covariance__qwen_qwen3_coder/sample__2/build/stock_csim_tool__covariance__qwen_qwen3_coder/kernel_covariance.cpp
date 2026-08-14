
#include "covariance.h"

void kernel_covariance(
    double float_n,
    double data[32][28],
    double cov[28][28],
    double mean[28]) {

    int i, j, k;

    // Step 1: Compute the mean for each attribute
    for (j = 0; j < 28; j++) {
        mean[j] = 0.0;
        for (k = 0; k < 32; k++) {
            mean[j] += data[k][j];
        }
        mean[j] /= float_n;
    }

    // Step 2: Compute the covariance matrix
    for (i = 0; i < 28; i++) {
        for (j = 0; j < 28; j++) {
            cov[i][j] = 0.0;
            for (k = 0; k < 32; k++) {
                cov[i][j] += (data[k][i] - mean[i]) * (data[k][j] - mean[j]);
            }
            cov[i][j] /= (float_n - 1.0);
        }
    }
}
