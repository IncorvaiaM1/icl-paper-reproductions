
#include "trisolv.h"

void kernel_trisolv(double L[40][40], double x[40], double b[40]) {
    // Forward substitution: solve L * x = b for x
    // L is lower triangular, so L[i][j] = 0 for j > i
    for (int i = 0; i < 40; i++) {
        double sum = 0.0;
        // Compute sum of L[i][j] * x[j] for j = 0 to i-1
        for (int j = 0; j < i; j++) {
            #pragma HLS PIPELINE
            sum += L[i][j] * x[j];
        }
        // Compute x[i] = (b[i] - sum) / L[i][i]
        x[i] = (b[i] - sum) / L[i][i];
    }
}
