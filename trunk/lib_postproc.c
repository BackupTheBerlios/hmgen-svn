#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "lib_hmgen.h"

#define HMGEN_MODULE "Normalize"

void hmg_normalize(unsigned char *map,
                   unsigned int normmin, unsigned int normmax,
                   unsigned int w, unsigned int h) {
    unsigned int x = 0, y, minval = 255, maxval = 0, v = 0;
    double f;

    for (y=0; y<h; y++) {
        progress_meter((int)round(y*50.0/(h-1)));
        for (x=0; x<w; x++) {
            v = map[x+y*w];
            minval = v<minval ? v : minval;
            maxval = v>maxval ? v : maxval;
        }
    }

    f = 1.0 * (normmax-normmin) / (maxval-minval);

    for (y=0; y<h; y++) {
        progress_meter(50 + (int)round(y*50.0/(h-1)));
        for (x=0; x<w; x++) {
            v = map[x+y*w];
            map[x+y*w] = round((v - minval) * f + normmin);
        }
    }
    progress_meter(-1);
}

#undef  HMGEN_MODULE
#define HMGEN_MODULE "Gaussian Blur"

void hmg_gaussian_blur(unsigned char *map, unsigned char *tempmap,
                       int blurx, int blury,
                       double blursigma,
                       unsigned int w, unsigned int h) {
    double *mask;
    double cx = 0, cy = 0, r = 0, m = 0, nv;
    int x, y, a, b, v;
    double cr = -1.0/(2.0*blursigma*blursigma);

    mask = calloc(blurx*blury, sizeof(double));

    memcpy(tempmap, map, w*h);

    for (y=0; y<blury; y++) {
        for (x=0; x<blurx; x++) {
            cx = (double) x - (double)(blurx-1) / 2.0;
            cy = (double) y - (double)(blury-1) / 2.0;
            r = cx*cx + cy*cy;
            m += exp(cr*r);
        }
    }
    m = 1.0/m;

    for (y=0; y<blury; y++) {
        for (x=0; x<blurx; x++) {
            cx = (double) x - (double)(blurx-1) / 2.0;
            cy = (double) y - (double)(blury-1) / 2.0;
            r = cx*cx + cy*cy;
            mask[y*blurx+x] = m * exp(cr*r);
        }
    }

    for (y=0; y<(int)h; y++) {
        progress_meter((int)round(y*100.0/(h-1)));
        for (x=0; x<(int)w; x++) {

            nv = 0.0;

            for (b=0; b<blury; b++) {
                int bb = y+b-blury/2;
                bb = bb>=(int)h ? h-1 : bb<0 ? 0 : bb;

                for (a=0; a<blurx; a++) {
                    int aa = x+a-blurx/2;
                    aa = aa>=(int)w ? w-1 : aa<0 ? 0 : aa;

                    v = tempmap[aa+bb*w];
                    nv += v * mask[a+b*blurx];
                }
            }

            nv = round(nv);
            nv = nv>255 ? 255 : nv<0 ? 0 : nv;
            map[x+y*w] = nv;
        }
    }
    progress_meter(-1);
    free(mask);
}


#undef  HMGEN_MODULE
#define HMGEN_MODULE "Invert"

void hmg_invert(unsigned char *map, unsigned int w, unsigned int h) {
    unsigned int x = 0, y;

    for (y=0; y<h; y++) {
        progress_meter((int)round(y*100.0/(h-1)));
        for (x=0; x<w; x++)
            map[x+y*w] = 255 - map[x+y*w];
    }
    progress_meter(-1);
}

#undef  HMGEN_MODULE
#define HMGEN_MODULE "Clip"

void hmg_clip(unsigned char *map,
              unsigned int clipmin, unsigned int clipmax,
              unsigned int w, unsigned int h) {
    unsigned int x, y;
    unsigned char v;

    for (y=0; y<h; y++) {
        progress_meter((int)round(y*100.0/(h-1)));
        for (x=0; x<w; x++) {
            v = map[x];
            v = v > clipmax ? clipmax : v;
            map[x] = v < clipmin ? clipmin : v;
        }
        map += w;
    }
    progress_meter(-1);
}

#undef  HMGEN_MODULE
#define HMGEN_MODULE "Crop"

void hmg_crop(unsigned char **pmap,
              unsigned int *width, unsigned int *height,
              unsigned int left, unsigned int right,
              unsigned int top, unsigned int bottom) {
    unsigned int y, w = *width, h = *height;
    unsigned char *map = *pmap, *src = map;

    src += left;
    src += top * w;

    for (y=0; y<(h-top-bottom); y++) {
        progress_meter((int)round(y*100.0/(h-1-top-bottom)));
        memcpy(map, src, w-left-right);
        map += w-left-right;
        src += w;
    }

    *width = w-left-right;
    *height = h-top-bottom;
    *pmap = realloc(*pmap, *width * *height);
    progress_meter(-1);
}
