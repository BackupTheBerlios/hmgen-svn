#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lib_hmgen.h"

unsigned char hmg_colormap[256][3];

static hmg_colormap_settings_t default_colormap_settings = {
    { 0, 32, 96, 192, 255, 255, 255, 255 },
    { { 0, 0, 96 }, { 0, 0, 192 }, { 0, 192, 0 }, { 192, 128, 128 },
      { 255, 255, 255 }, { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } }
};

static void do_gradient(unsigned char start[3], unsigned char end[3],
                        int first, int last) {
    double d[3];
    int steps = last - first, i;

    if (!steps)     /* no divide by zero */
        return;

    d[0] = (double)(end[0] - start[0]) / steps;
    d[1] = (double)(end[1] - start[1]) / steps;
    d[2] = (double)(end[2] - start[2]) / steps;

    for (i=first; i<=last; i++) {
        int j = i-first;
        hmg_colormap[i][0] = round((double)start[0] + d[0]*j);
        hmg_colormap[i][1] = round((double)start[1] + d[1]*j);
        hmg_colormap[i][2] = round((double)start[2] + d[2]*j);
    }
}

void hmg_init_colormap(hmg_colormap_settings_t *settings) {
    hmg_colormap_settings_t *set = settings;
    int i;

    if (!set)
        set =&default_colormap_settings;

    for (i=0; i<7; i++)
        do_gradient(set->colors[i], set->colors[i+1], set->scales[i],
                    set->scales[i+1]);
}

#define nrand 4

double arand;
static double gaussadd, gaussfac;

void hmg_rng_initgauss(unsigned int seed) {
    arand = pow(2.0, 15.0) - 1.0;
    gaussadd = sqrt(3.0 * nrand);
    gaussfac = 2 * gaussadd / (nrand * nrand);
    srand(seed);
}

double hmg_rng_gauss(void) {
    int i;
    double sum = 0.0;

    for (i=1; i<=nrand; i++)
        sum += rand() & 0x7fff;
    return gaussfac * sum - gaussadd;
}
