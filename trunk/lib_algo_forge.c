/* This code is mostly taken from ppmforge.c
 * Author: John Walker http://www.fourmilab.ch/ (C)1989
 *
 * Permission  to  use, copy, modify, and distribute this software and
 * its documentation  for  any  purpose  and  without  fee  is  hereby
 * granted,  without any conditions or restrictions.  This software is
 * provided "as is" without express or implied warranty.
 *
 * Modifications and adaptation by Ivo van Poorten (C)2007
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _XOPEN_SOURCE
#define __USE_XOPEN
#include <math.h>
#include <assert.h>
#include "lib_hmgen.h"

#define HMGEN_MODULE "Forge"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define hugeVal HUGE_VAL

#define MIN(a,b)    (a<b?a:b)
#define MAX(a,b)    (a>b?a:b)

extern double arand;

static struct forge_settings_s {
    unsigned int seed;
    double fracdim;
    double powscale;
} forge_settings = { 47, 2.4, 1.2 };

static const hmg_param_t params_forge[] = {
    { "seed=", &forge_settings.seed, 0.0, 4294967295.0, PARAM_INT },
    { "fracdim=", &forge_settings.fracdim, 0.0, 3.0, PARAM_DOUBLE },
    { "powscale=", &forge_settings.powscale, 0.0, 8.0, PARAM_DOUBLE },
    { NULL, NULL, 0, 0, 0 }
};

static int meshsize;

#define Real(v, x, y)  v[1 + (((x) * meshsize) + (y)) * 2]
#define Imag(v, x, y)  v[2 + (((x) * meshsize) + (y)) * 2]

/* FOURN  --  Multi-dimensional fast Fourier transform
 *
 * data     A one-dimensional array of floats (NOTE!!! NOT DOUBLES!!)
 *          indexed from one (NOTE!!! NOT ZERO!!), containing  pairs of
 *          numbers representing the complex valued samples. The Fourier
 *          transformed results  are returned in the same array.
 *
 * nn       An  array specifying the edge size in each dimension. THIS ARRAY
 *          IS INDEXED FROM ONE, AND ALL THE EDGE SIZES MUST BE POWERS OF TWO!!
 *
 * ndim     Number of dimensions of FFT to perform. Set to 2 for 2D FFT.
 *
 * isign    If 1, a Fourier transform is done; if -1 the  inverse
 *          transformation is performed.
 *
 * This  function  is essentially as given in Press et al., "Numerical
 * Recipes In C", Section 12.11, pp.  467-470.
 */

static void fourn(data, nn, ndim, isign)
    float data[];
    int nn[], ndim, isign;
{
    register int i1, i2, i3;
    int i2rev, i3rev, ip1, ip2, ip3, ifp1, ifp2;
    int ibit, idim, k1, k2, n, nprev, nrem, ntot;
    float tempi, tempr;
    double theta, wi, wpi, wpr, wr, wtemp;

#define SWAP(a,b) tempr=(a); (a) = (b); (b) = tempr

    ntot = 1;
    for (idim = 1; idim <= ndim; idim++)
        ntot *= nn[idim];
    nprev = 1;
    for (idim = ndim; idim >= 1; idim--) {
        /* FIXME: this makes only two steps */
        progress_meter(66+(int)round((ndim-idim)*34.0/ndim));

        n = nn[idim];
        nrem = ntot / (n * nprev);
        ip1 = nprev << 1;
        ip2 = ip1 * n;
        ip3 = ip2 * nrem;
        i2rev = 1;
        for (i2 = 1; i2 <= ip2; i2 += ip1) {
            if (i2 < i2rev) {
                for (i1 = i2; i1 <= i2 + ip1 - 2; i1 += 2) {
                    for (i3 = i1; i3 <= ip3; i3 += ip2) {
                        i3rev = i2rev + i3 - i2;
                        SWAP(data[i3], data[i3rev]);
                        SWAP(data[i3 + 1], data[i3rev + 1]);
                    }
                }
            }
            ibit = ip2 >> 1;
            while (ibit >= ip1 && i2rev > ibit) {
                i2rev -= ibit;
                ibit >>= 1;
            }
            i2rev += ibit;
        }
        ifp1 = ip1;
        while (ifp1 < ip2) {
            ifp2 = ifp1 << 1;
            theta = isign * (M_PI * 2) / (ifp2 / ip1);
            wtemp = sin(0.5 * theta);
            wpr = -2.0 * wtemp * wtemp;
            wpi = sin(theta);
            wr = 1.0;
            wi = 0.0;
            for (i3 = 1; i3 <= ifp1; i3 += ip1) {
                for (i1 = i3; i1 <= i3 + ip1 - 2; i1 += 2) {
                    for (i2 = i1; i2 <= ip3; i2 += ifp2) {
                        k1 = i2;
                        k2 = k1 + ifp1;
                        tempr = wr * data[k2] - wi * data[k2 + 1];
                        tempi = wr * data[k2 + 1] + wi * data[k2];
                        data[k2] = data[k1] - tempr;
                        data[k2 + 1] = data[k1 + 1] - tempi;
                        data[k1] += tempr;
                        data[k1 + 1] += tempi;
                    }
                }
                wr = (wtemp = wr) * wpr - wi * wpi + wr;
                wi = wi * wpr + wtemp * wpi + wi;
            }
            ifp1 = ifp2;
        }
        nprev *= n;
    }
}
#undef SWAP

/* SPECTRALSYNTH  --  Spectrally synthesized fractal motion in two
 * dimensions. This algorithm is given under the name SpectralSynthesisFM2D
 * on page 108 of Peitgen & Saupe.
 */

static void spectral_synth(float **x, int n, double h) {
    unsigned int bl;
    int i, j, i0, j0, nsize[3];
    double rad, phase, rcos, rsin;
    float *a;

    bl = ((((unsigned long) n) * n) + 1) * 2 * sizeof(float);
    a = (float *) calloc(bl, 1);
    if (a == (float *) 0) {
        fprintf(stderr, "error, cannot allocate memory\n");
        exit(1);
    }
    *x = a;

    for (i = 0; i <= n / 2; i++) {
        progress_meter((int)round(i*33.0/((n/2)-1)));
        for (j = 0; j <= n / 2; j++) {
            phase = 2 * M_PI * ((rand() & 0x7FFF) / arand);
            if (i != 0 || j != 0) {
                rad = pow((double)(i*i + j*j), -(h + 1) / 2) * hmg_rng_gauss();
            } else {
                rad = 0;
            }
            rcos = rad * cos(phase);
            rsin = rad * sin(phase);
            Real(a, i, j) = rcos;
            Imag(a, i, j) = rsin;
            i0 = (i == 0) ? 0 : n - i;
            j0 = (j == 0) ? 0 : n - j;
            Real(a, i0, j0) = rcos;
            Imag(a, i0, j0) = - rsin;
        }
    }
    Imag(a, n / 2, 0) = 0;
    Imag(a, 0, n / 2) = 0;
    Imag(a, n / 2, n / 2) = 0;
    for (i = 1; i <= n / 2 - 1; i++) {
        progress_meter(33 + (int)round(i*33.0/((n/2)-1)));
        for (j = 1; j <= n / 2 - 1; j++) {
            phase = 2 * M_PI * ((rand() & 0x7FFF) / arand);
            rad = pow((double)(i*i + j*j), -(h + 1) / 2) * hmg_rng_gauss();
            rcos = rad * cos(phase);
            rsin = rad * sin(phase);
            Real(a, i, n - j) = rcos;
            Imag(a, i, n - j) = rsin;
            Real(a, n - i, j) = rcos;
            Imag(a, n - i, j) = - rsin;
        }
    }

    nsize[0] = 0;
    nsize[1] = nsize[2] = n;          /* Dimension of frequency domain array */
    fourn(a, nsize, 2, -1);       /* Take inverse 2D Fourier transform */
}

static void power_law_scaling(float * const a, int const meshsize,
                              double const powscale) {

    if (powscale != 1.0) {
        int i, j;
        for (i = 0; i < meshsize; i++) {
            for (j = 0; j < meshsize; j++) {
                double const r = Real(a, i, j);
                if (r > 0)
                    Real(a, i, j) = pow(r, powscale);
            }
        }
    }
}

static void compute_extreme_real(const float * const a, int const meshsize,
                                 double * const rminP, double * const rmaxP) {
    double rmin, rmax;
    int i, j;

    rmin = hugeVal;
    rmax = -hugeVal;

    for (i = 0; i < meshsize; i++) {
        for (j = 0; j < meshsize; j++) {
            double r = Real(a, i, j);

            rmin = MIN(rmin, r);
            rmax = MAX(rmax, r);
        }
    }
    *rminP = rmin;
    *rmaxP = rmax;
}

/* Replace the real part of each element of the 'a' array with a
 * measure of how far the real is from the middle; sort of a standard
 * deviation.
 */
static void replace_with_spread(float * const a, int const meshsize) {
    double rmin, rmax;
    double rmean, rrange;
    int i, j;

    compute_extreme_real(a, meshsize, &rmin, &rmax);

    rmean = (rmin + rmax) / 2;
    rrange = (rmax - rmin) / 2;

    for (i = 0; i < meshsize; i++) {
        for (j = 0; j < meshsize; j++) {
            Real(a, i, j) = (Real(a, i, j) - rmean) / rrange;
        }
    }
}

static void algo_forge(unsigned char *map,
                       unsigned char *tempmap HMG_ATTR_UNUSED,
                       unsigned int w, unsigned int h) {
    float *a;
    unsigned int x, y;

    hmg_rng_initgauss(forge_settings.seed);

    assert(w == h);
    meshsize = w-1;

    spectral_synth(&a, meshsize, 3.0 - forge_settings.fracdim);
    assert(a);

    power_law_scaling(a, meshsize, forge_settings.powscale);
    replace_with_spread(a, meshsize);

    for (y=0; y<h-1; y++)
        for(x=0; x<w-1; x++)
            map[x+y*w] = round ((Real(a,x,y) + 1.0) * 127.5);

    if (a) free(a);
    progress_meter(100);
    progress_meter(-1);
}

const hmg_algorithm_t hmg_algo_forge_info = {
    "ppmforge/gforge - noise/fft",
    algo_forge,
    params_forge
};

