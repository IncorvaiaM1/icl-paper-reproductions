
#include "fft_strided.h"

void fft(double real[FFT_SIZE], double img[FFT_SIZE], double real_twid[FFT_SIZE / 2], double img_twid[FFT_SIZE / 2]) {
    int span, odd, rootindex;
    double temp;

    // Outer loop: iterates over the span of the input sequence
    for (span = FFT_SIZE >> 1; span; span >>= 1) {
        // Inner loop: iterates over the odd indices of the input sequence
        for (odd = span; odd < FFT_SIZE; odd++) {
            // Calculate rootindex for accessing twiddle factors
            rootindex = (odd - span) * (FFT_SIZE / 2) / span;

            // Twiddle factor multiplication
            temp = real_twid[rootindex] * real[odd] - img_twid[rootindex] * img[odd];
            img[odd] = real_twid[rootindex] * img[odd] + img_twid[rootindex] * real[odd];
            real[odd] = temp;

            // Butterfly operation
            temp = real[odd - span] + real[odd];
            real[odd] = real[odd - span] - real[odd];
            real[odd - span] = temp;

            temp = img[odd - span] + img[odd];
            img[odd] = img[odd - span] - img[odd];
            img[odd - span] = temp;
        }
    }
}
