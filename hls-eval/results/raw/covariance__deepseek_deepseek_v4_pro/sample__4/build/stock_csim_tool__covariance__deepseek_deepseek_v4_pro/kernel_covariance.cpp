
#include "covariance.h"

void kernel_covariance(
    double float_n,
    double data[32][28],
    double cov[28][28],
    double mean[28]) {

    int n = (int)float_n;

    // Compute the mean of each attribute (column)
    for (int j = 0; j < 28; j++) {
        double sum = 0.0;
        for (int k = 0; k < n; k++) {
            sum += data[k][j];
        }
        mean[j] = sum / float_n;
    }

    // Compute the covariance matrix
    for (int i = 0; i < 28; i++) {
        for (int j = 0; j < 28; j++) {
            double sum = 0.0;
            for (int k = 0; k < n; k++) {
                double di = data[k][i] - mean[i];
                double dj = data[k][j] - mean[j];
                sum += di * dj;
            }
            cov[i][j] = sum / (float_n - 1.0);
        }
    }
}
