#include <stdio.h>
#include <stdlib.h>
#include <math.h>

unsigned char hmg_colormap[256][3];

#define B_WATER_GRASS    32
#define B_GRASS_ROCK     96
#define B_ROCK_SNOW      192

#if 0
static unsigned char waterstart[3] = {   0,   0, 128 },
                     waterend[3]   = {   0,   0, 255 },
                     grassstart[3] = {   0, 128,   0 },
                     grassend[3]   = {   0, 255,   0 },
                     rockstart[3]  = { 128,   0,   0 },
                     rockend[3]    = { 255, 128, 128 },
                     snowstart[3]  = { 128, 128, 128 },
                     snowend[3]    = { 255, 255, 255 };
#endif
static unsigned char waterstart[3] = {   0,   0, 96 },
                     waterend[3]   = {   0,   0, 192 },
                     grassstart[3] = {   0, 0,  192 },
                     grassend[3]   = {   0, 192,   0 },
                     rockstart[3]  = { 0,   192,   0 },
                     rockend[3]    = { 192, 128, 128 },
                     snowstart[3]  = { 192, 128, 128 },
                     snowend[3]    = { 255, 255, 255 };

// colormap[last] is NOT set
static void do_gradient(unsigned char start[3], unsigned char end[3],
                        int first, int last) {
    double d[3];
    int steps = last - first - 1, i;

    d[0] = (double)(end[0] - start[0]) / steps;
    d[1] = (double)(end[1] - start[1]) / steps;
    d[2] = (double)(end[2] - start[2]) / steps;

    for (i=first; i<last; i++) {
        int j = i-first;
        hmg_colormap[i][0] = round((double)start[0] + d[0]*j);
        hmg_colormap[i][1] = round((double)start[1] + d[1]*j);
        hmg_colormap[i][2] = round((double)start[2] + d[2]*j);
    }
}

void hmg_init_colormap(void) {
    do_gradient(waterstart, waterend, 0,             B_WATER_GRASS);
    do_gradient(grassstart, grassend, B_WATER_GRASS, B_GRASS_ROCK);
    do_gradient(rockstart,  rockend,  B_GRASS_ROCK,  B_ROCK_SNOW);
    do_gradient(snowstart,  snowend,  B_ROCK_SNOW,   256);
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
