
#include "durbin.h"

void kernel_durbin(double r[40], double y[40]) {
    int i, k;
    double alpha, beta, sum;

    double z[40];

    y[0] = -r[0];
    beta = 1.0;
    alpha = -r[0];

    loop_z: for (k = 1; k < 40; k++) {
        beta = (1 - alpha * alpha) * beta;
        sum = 0.0;

        loop_sum: for (i = 0; i < k; i++) {
            sum += r[k - i - 1] * y[i];
        }

        alpha = -(r[k] + sum) / beta;

        loop_y: for (i = 0; i < k; i++) {
            z[i] = y[i] + alpha * y[k - i - 1];
        }

        loop_assign: for (i = 0; i < k; i++) {
            y[i] = z[i];
        }

        y[k] = alpha;
    }
}
