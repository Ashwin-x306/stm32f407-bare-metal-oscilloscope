#ifndef FFT_H
#define FFT_H

#include <stdint.h>

#define FFT_SIZE 2048  

typedef struct {
    float re;
    float im;
} Complex;

void fft_compute(Complex *x, uint16_t n);
float fft_find_dominant_freq(uint16_t *raw_samples, uint16_t n, float sample_rate);

#endif
