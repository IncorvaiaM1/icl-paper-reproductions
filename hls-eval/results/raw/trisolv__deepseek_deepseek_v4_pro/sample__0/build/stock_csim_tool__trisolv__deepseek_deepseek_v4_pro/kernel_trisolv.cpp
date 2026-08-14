
#include "trisolv.h"

void kernel_trisolv(double L[40][40], double x[40], double b[40]) {
    // Forward substitution: solve L * x = b for lower triangular L
    for (int i = 0; i < 40; i++) {
        double sum = 0.0;
        // Accumulate L[i][j] * x[j] for j < i
        for (int j = 0; j < i; j++) {
            sum += L[i][j] * x[j];
        }
        // Compute x[i] = (b[i] - sum) / L[i][i]
        x[i] = (b[i] - sum) / L[i][i];
    }
}
