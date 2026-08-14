
#include "correlation.h"

void kernel_correlation(double float_n, double data[32][28], double corr[28][28], double mean[28], double stddev[28]) {
    // Compute mean for each attribute (column)
    for (int j = 0; j < 28; j++) {
        mean[j] = 0.0;
        for (int i = 0; i < 32; i++) {
            mean[j] += data[i][j];
        }
        mean[j] /= float_n;
    }

    // Compute standard deviation for each attribute
    for (int j = 0; j < 28; j++) {
        double sum_sq = 0.0;
        for (int i = 0; i < 32; i++) {
            double diff = data[i][j] - mean[j];
            sum_sq += diff * diff;
        }
        stddev[j] = sqrt(sum_sq / float_n);
    }

    // Compute correlation matrix
    for (int i = 0; i < 28; i++) {
        for (int j = 0; j < 28; j++) {
            double sum = 0.0;
            for (int k = 0; k < 32; k++) {
                double diff_i = data[k][i] - mean[i];
                double diff_j = data[k][j] - mean[j];
                sum += diff_i * diff_j;
            }
            double cov = sum / float_n;
            // Handle division by zero cases
            if (stddev[i] < 1e-10 || stddev[j] < 1e-10) {
                corr[i][j] = 0.0;
            } else {
                corr[i][j] = cov / (stddev[i] * stddev[j]);
            }
        }
    }
}
