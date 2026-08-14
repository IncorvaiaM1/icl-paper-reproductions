
#include "block.h"

void BlockFrequency(double *result, int epsilon[M * N]) {
    double sum = 0.0;
    for (int i = 0; i < M; ++i) {
        int blockSum = 0;
        for (int j = 0; j < N; ++j) {
            blockSum += epsilon[i * N + j];
        }
        double pi = static_cast<double>(blockSum) / M;
        double v = pi - 0.5;
        sum += v * v;
    }
    *result = sum;
}
