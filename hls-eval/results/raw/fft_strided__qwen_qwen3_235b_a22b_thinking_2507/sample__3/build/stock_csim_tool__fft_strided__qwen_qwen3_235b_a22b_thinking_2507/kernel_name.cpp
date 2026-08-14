
#include "fft_strided.h"

void fft(
    double real[FFT_SIZE],
    double img[FFT_SIZE],
    double real_twid[FFT_SIZE / 2],
    double img_twid[FFT_SIZE / 2]) {
    
    int log = 0;
    for (int span = FFT_SIZE >> 1; span; span >>= 1, log++) {
        int factor = FFT_SIZE / (2 * span);
        
        for (int i = 0; i < FFT_SIZE; i += 2 * span) {
            for (int j = 0; j < span; j++) {
                int even = i + j;
                int odd = i + j + span;
                int rootindex = j * factor;
                
                double temp_real = real_twid[rootindex] * real[odd] - img_twid[rootindex] * img[odd];
                double temp_img = real_twid[rootindex] * img[odd] + img_twid[rootindex] * real[odd];
                
                double save_real = real[even];
                double save_img = img[even];
                
                real[even] = save_real + temp_real;
                img[even] = save_img + temp_img;
                real[odd] = save_real - temp_real;
                img[odd] = save_img - temp_img;
            }
        }
    }
}
