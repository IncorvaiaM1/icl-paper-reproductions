
#include "covariance.h"

void kernel_covariance(double float_n, double data[32][28], double cov[28][28], double mean[28]) {
    int N = (int)float_n;

    // Compute the mean for each attribute
    for (int j = 0; j < 28; j++) {
        mean[j] = 0.0;
        for (int i = 0; i < N; i++) {
            mean[j] += data[i][j];
        }
        mean[j] /= float_n;
    }

    double denom = float_n - 1.0;
    // Compute the covariance matrix
    for (int i = 0; i < 28; i++) {
        for (int j = 0; j < 28; j++) {
            double sum_val = 0.0;
            for (int k = 0; k < N; k++) {
                double diff_i = data[k][i] - mean[i];
                double diff_j = data[k][j] - mean[j];
                sum_val += diff_i * diff_j;
            }
            cov[i][j] = sum_val / denom;
        }
    }
}
