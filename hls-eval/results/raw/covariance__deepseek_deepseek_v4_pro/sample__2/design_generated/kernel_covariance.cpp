
#include "covariance.h"

void kernel_covariance(double float_n, double data[32][28], double cov[28][28], double mean[28]) {
    int n = (int) float_n;

    double inv_n = 1.0 / float_n;
    double inv_n_minus_1 = 1.0 / (float_n - 1.0);

    // Compute mean of each attribute.
    for (int j = 0; j < 28; j++) {
        double sum = 0.0;
        for (int i = 0; i < 32; i++) {
            if (i < n) {
                sum += data[i][j];
            }
        }
        mean[j] = sum * inv_n;
    }

    // Compute covariance matrix.
    for (int i = 0; i < 28; i++) {
        for (int j = 0; j < 28; j++) {
            double sum = 0.0;
            for (int k = 0; k < 32; k++) {
                if (k < n) {
                    double di = data[k][i] - mean[i];
                    double dj = data[k][j] - mean[j];
                    sum += di * dj;
                }
            }
            cov[i][j] = sum * inv_n_minus_1;
        }
    }
}
