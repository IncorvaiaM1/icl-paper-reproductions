
#include "fft_strided.h"

void fft(double real[FFT_SIZE], double img[FFT_SIZE], double real_twid[FFT_SIZE / 2], double img_twid[FFT_SIZE / 2]) {
    int span, odd, rootindex;
    double temp;
    int log = 0;

    // Outer loop that iterates over the span of the input sequence
    for (span = FFT_SIZE >> 1; span; span >>= 1, log++) {
        // Inner loop that iterates over the odd indices of the input sequence
        for (odd = span; odd < FFT_SIZE; odd++) {
            // Compute rootindex based on current position and log
            rootindex = (odd & (span - 1)) << (log + 1);

            // Perform twiddle factor multiplication
            temp = real_twid[rootindex] * real[odd] - img_twid[rootindex] * img[odd];
            img[odd] = real_twid[rootindex] * img[odd] + img_twid[rootindex] * real[odd];
            real[odd] = temp;

            // Butterfly operation
            temp = real[odd - span];
            real[odd - span] = temp + real[odd];
            real[odd] = temp - real[odd];

            temp = img[odd - span];
            img[odd - span] = temp + img[odd];
            img[odd] = temp - img[odd];
        }
    }
}
