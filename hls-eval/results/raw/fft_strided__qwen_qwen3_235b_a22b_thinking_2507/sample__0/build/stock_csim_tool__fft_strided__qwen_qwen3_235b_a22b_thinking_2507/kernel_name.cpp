
#include "fft_strided.h"

void fft(double real[FFT_SIZE], double img[FFT_SIZE], 
         double real_twid[FFT_SIZE / 2], double img_twid[FFT_SIZE / 2]) {
    for (int span = FFT_SIZE >> 1; span; span >>= 1) {
        int step = FFT_SIZE / (2 * span);
        for (int odd = span; odd < FFT_SIZE; odd++) {
            int even = odd - span;
            double t_real = real[even] - real[odd];
            double t_img = img[even] - img[odd];
            real[even] += real[odd];
            img[even] += img[odd];
            
            int k = (odd - span) & ((2 * span) - 1);
            int exponent = k * step;
            double factor_real, factor_img;
            if (exponent >= FFT_SIZE / 2) {
                int rootindex = exponent - FFT_SIZE / 2;
                factor_real = -real_twid[rootindex];
                factor_img = -img_twid[rootindex];
            } else {
                factor_real = real_twid[exponent];
                factor_img = img_twid[exponent];
            }
            
            real[odd] = factor_real * t_real - factor_img * t_img;
            img[odd] = factor_real * t_img + factor_img * t_real;
        }
    }
}
