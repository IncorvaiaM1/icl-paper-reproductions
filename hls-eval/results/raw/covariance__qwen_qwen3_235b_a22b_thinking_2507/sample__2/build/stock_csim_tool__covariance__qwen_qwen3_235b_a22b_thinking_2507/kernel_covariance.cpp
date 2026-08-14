
#include "covariance.h"

void kernel_covariance(double float_n, double data[32][28], double cov[28][28], double mean[28]) {
    // Step 1: Compute mean for each attribute (column)
    for (int j = 0; j < 28; j++) {
        double sum = 0.0;
        for (int i = 0; i < 32; i++) {
            sum += data[i][j];
        }
        mean[j] = sum / float_n;
    }

    // Step 2: Compute covariance matrix
    for (int i = 0; i < 28; i++) {
        for (int j = 0; j < 28; j++) {
            double sum = 0.0;
            for (int k = 0; k < 32; k++) {
                double diff_i = data[k][i] - mean[i];
                double diff_j = data[k][j] - mean[j];
                sum += diff_i * diff_j;
            }
            cov[i][j] = sum / (float_n - 1.0);
        }
    }
}
