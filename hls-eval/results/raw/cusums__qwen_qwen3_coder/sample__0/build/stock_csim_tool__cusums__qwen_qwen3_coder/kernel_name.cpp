
#include "cusums.h"

void CumulativeSums(int *res_sup, int *res_inf, int epsilon[N]) {
    int S = 0;
    int sup = 0;
    int inf = 0;

    for (int k = 0; k < N; k++) {
        if (epsilon[k] == 1) {
            S++;
        } else {
            S--;
        }

        if (k == 0) {
            sup = S;
            inf = S;
        } else {
            if (S > sup) {
                sup = S;
            }
            if (S < inf) {
                inf = S;
            }
        }
    }

    *res_sup = sup;
    *res_inf = inf;
}
