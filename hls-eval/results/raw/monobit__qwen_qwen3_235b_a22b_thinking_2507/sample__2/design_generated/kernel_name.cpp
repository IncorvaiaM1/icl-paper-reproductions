
#include "monobit.h"

void Frequency(int *result, int epsilon[N]) {
    int total = 0;
    for (int i = 0; i < N; i++) {
        total += (2 * epsilon[i] - 1);
    }
    *result = total;
}
