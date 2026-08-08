#include "fft.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static void bit_reverse(Complex *x, uint16_t n){
    uint16_t j = 0;
    for (uint16_t i = 0; i < n - 1; i++){
        if (i < j)
        {
            Complex temp = x[i];
            x[i] = x[j];
            x[j] = temp;
        }
        uint16_t k = n >> 1;
        while (k <= j){
            j -= k;
            k >>= 1;
        }
        j += k;
    }
}


void fft_compute(Complex *x, uint16_t n)
{
    bit_reverse(x, n);

    for (uint16_t len = 2; len <= n; len <<= 1)
    {
        float angle = -2.0f * M_PI / (float)len;
        Complex wlen = { cos(angle), sin(angle) };

        for (uint16_t i = 0; i < n; i += len)
        {
            Complex w = { 1.0f, 0.0f };
            for (uint16_t j = 0; j < len / 2; j++)
            {
                Complex u = x[i + j];
                Complex v;
         
                v.re = x[i + j + len / 2].re * w.re - x[i + j + len / 2].im * w.im;
                v.im = x[i + j + len / 2].re * w.im + x[i + j + len / 2].im * w.re;

              
                x[i + j].re          = u.re + v.re;
                x[i + j].im          = u.im + v.im;
                x[i + j + len/2].re  = u.re - v.re;
                x[i + j + len/2].im  = u.im - v.im;

                float w_re_new = w.re * wlen.re - w.im * wlen.im;
                float w_im_new = w.re * wlen.im + w.im * wlen.re;
                w.re = w_re_new;
                w.im = w_im_new;
            }
        }
    }
}

float fft_find_dominant_freq(uint16_t *raw_samples, uint16_t n, float sample_rate)
{
    static Complex x[FFT_SIZE];

    float mean = 0.0f;
    for (uint16_t i = 0; i < n; i++) mean += (float)raw_samples[i];
    mean /= (float)n;

    for (uint16_t i = 0; i < n; i++)
    {
        float w = 0.5f * (1.0f - cos(2.0f * M_PI * i / (float)(n - 1)));
        x[i].re = ((float)raw_samples[i] - mean) * w;
        x[i].im = 0.0f;
    }


    fft_compute(x, n);

    uint16_t peak_k = 1;
    float peak_mag = 0.0f;

    for (uint16_t k = 1; k < n / 2; k++)
    {
        float mag = sqrt(x[k].re * x[k].re + x[k].im * x[k].im);
        if (mag > peak_mag)
        {
            peak_mag = mag;
            peak_k = k;
        }
    }


    return (float)peak_k * sample_rate / (float)n;
}
