
#include "monobit.h"

void Frequency(int *result, int epsilon[N]) {
    int sum = 0;
    for (int i = 0; i < N; i++) {
        sum += 2 * epsilon[i] - 1;
    }
    *result = sum;
}
