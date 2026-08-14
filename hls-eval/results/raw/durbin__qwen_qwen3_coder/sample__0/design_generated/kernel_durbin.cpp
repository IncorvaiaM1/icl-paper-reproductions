
#include "durbin.h"

void kernel_durbin(double r[40], double y[40]) {
    double alpha, beta, sum;
    double z[40];

    y[0] = -r[0];
    beta = 1.0;
    alpha = -r[0];

    for (int k = 1; k < 40; k++) {
        beta = (1 - alpha * alpha) * beta;
        sum = 0.0;

        for (int i = 0; i < k; i++) {
            sum += r[k - i - 1] * y[i];
        }

        alpha = -(r[k] + sum) / beta;

        for (int i = 0; i < k; i++) {
            z[i] = y[i] + alpha * y[k - i - 1];
        }

        for (int i = 0; i < k; i++) {
            y[i] = z[i];
        }

        y[k] = alpha;
    }
}
