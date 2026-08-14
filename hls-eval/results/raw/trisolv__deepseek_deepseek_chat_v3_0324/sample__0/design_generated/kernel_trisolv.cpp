
#include "trisolv.h"

void kernel_trisolv(double L[40][40], double x[40], double b[40]) {
    #pragma HLS INTERFACE m_axi port=L bundle=gmem0
    #pragma HLS INTERFACE m_axi port=x bundle=gmem1
    #pragma HLS INTERFACE m_axi port=b bundle=gmem2

    double sum;
    
    for (int i = 0; i < 40; i++) {
        #pragma HLS PIPELINE II=1
        sum = b[i];
        
        for (int j = 0; j < i; j++) {
            #pragma HLS LOOP_FLATTEN
            sum -= L[i][j] * x[j];
        }
        
        x[i] = sum / L[i][i];
    }
}
