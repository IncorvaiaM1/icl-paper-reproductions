
#include "correlation.h"

void kernel_correlation(
    double float_n,
    double data[32][28],
    double corr[28][28],
    double mean[28],
    double stddev[28])
{
    // Compute mean of each attribute (column)
    for (int j = 0; j < 28; j++) {
        mean[j] = 0.0;
        for (int i = 0; i < 32; i++) {
            mean[j] += data[i][j];
        }
        mean[j] /= float_n;
    }

    // Compute standard deviation of each attribute
    for (int j = 0; j < 28; j++) {
        double sum_sq = 0.0;
        for (int i = 0; i < 32; i++) {
            double diff = data[i][j] - mean[j];
            sum_sq += diff * diff;
        }
        stddev[j] = sqrt(sum_sq / float_n);
    }

    // Normalize the data
    double norm[32][28];
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 28; j++) {
            norm[i][j] = (data[i][j] - mean[j]) / stddev[j];
        }
    }

    // Compute correlation coefficients
    for (int i = 0; i < 28; i++) {
        for (int j = 0; j < 28; j++) {
            double sum = 0.0;
            for (int k = 0; k < 32; k++) {
                sum += norm[k][i] * norm[k][j];
            }
            corr[i][j] = sum / float_n;
        }
    }
}
