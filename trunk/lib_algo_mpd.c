/*

    hmgen - Height Map Generator
    Copyright (C) 2007 Ivo van Poorten

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lib_hmgen.h"

#define HMGEN_MODULE "Midpoint Displacement"

static struct mpd_settings_s {
    unsigned int seed;
    unsigned char tl, tr, bl, br;   /* top-left --> bottom-right */
    unsigned char d;                /* displacement */
    double r;                       /* roughness factor */
} mpd_settings = { 42, 128, 128, 128, 128, 16, 0.6f };

static const hmg_param_t params_mpd[] = {
    { "seed=", &mpd_settings.seed, 0.0, 4294967295.0, PARAM_INT },
    { "tl=",   &mpd_settings.tl, 0.0, 255.0, PARAM_CHAR },
    { "tr=",   &mpd_settings.tr, 0.0, 255.0, PARAM_CHAR },
    { "bl=",   &mpd_settings.bl, 0.0, 255.0, PARAM_CHAR },
    { "br=",   &mpd_settings.br, 0.0, 255.0, PARAM_CHAR },
    { "d=",    &mpd_settings.d,  0.0, 255.0, PARAM_CHAR },
    { "r=",    &mpd_settings.r,  0.0, 2.0,  PARAM_DOUBLE },
    { NULL, NULL, 0, 0, 0 }
};

static inline unsigned char getpix(unsigned char *map,
                                   int w, int h, int x, int y) {
    if (x<0) x += w-1;
    else if (x>=w) x -= w-1;
    if (y<0) y += h-1;
    else if (y>=h) y -= h-1;
    return map[x+y*w];
}

static inline void putpix(unsigned char *map,
                          int w, int h, int x, int y, int c) {
    if (x<0) x += w-1;
    else if (x>=w) x -= w-1;
    if (y<0) y += h-1;
    else if (y>=h) y -= h-1;
    if (c<0) c=0;
    else if (c>255) c=255;
    map[x+y*w] = c;
}

static inline char myrand(unsigned int d) {
    return rand() % (2*d) - d;
}

static void mpd(unsigned char *map, int x, int y,
                unsigned int xw, unsigned int yh,
                unsigned int w, unsigned int h,
                unsigned char d, unsigned int m) {
    int tl = getpix(map, w, h, x, y),
        tr = getpix(map, w, h, x+xw, y),
        bl = getpix(map, w, h, x, y+yh),
        br = getpix(map, w, h, x+xw, y+yh),
        midu, midd, midl, midr, mid;

    xw /= 2;
    yh /= 2;

    if (m) {
        mid = round((tl+tr+bl+br)/4.0 + myrand(d));
        putpix(map, w, h, x+xw, y+yh, mid);

        return;
    }

    mid  = getpix(map, w, h, x+xw, y+yh);
    midu = getpix(map, w, h, x+xw, y-yh);
    midd = getpix(map, w, h, x+xw, y+yh+yh+yh);
    midl = getpix(map, w, h, x-xw, y+yh);
    midr = getpix(map, w, h, x+xw+xw+xw, y+yh);

    putpix(map, w, h, x+xw,    y,       round((tl+tr+mid+midu)/4.0 +myrand(d)));
    putpix(map, w, h, x,       y+yh,    round((tl+bl+mid+midl)/4.0 +myrand(d)));
    putpix(map, w, h, x+xw+xw, y+yh,    round((tr+br+mid+midr)/4.0 +myrand(d)));
    putpix(map, w, h, x+xw,    y+yh+yh, round((bl+br+mid+midd)/4.0 +myrand(d)));
}

static void algo_mpd(unsigned char *map,
                     unsigned char *tempmap HMG_ATTR_UNUSED,
                     unsigned int w, unsigned int h) {
    unsigned int s = w-1, t = h-1, x, y, cnt = 0, tot = 0;
    double dd = mpd_settings.d;
    unsigned char d;

    /* calculate total number of calls to mpd() for progress counter */
    y = s/2;
    while (y!=1) {
        tot += y*y;
        y /= 2;
    }
    tot++;
    tot *= 2;

    srand(mpd_settings.seed);

    map[0]       = mpd_settings.tl;
    map[w-1]     = mpd_settings.tr;
    map[(h-1)*w] = mpd_settings.bl;
    map[h*w-1]   = mpd_settings.br;

    while (s != 1) {
        d = round(dd);
        d = d<1 ? 1 : d;
        for (y=0; y<h-1; y+=t) {
            progress_meter((int)round(cnt*100.0/tot));
            for (x=0; x<w-1; x+=s) {
                mpd(map, x, y, s, t, w, h, d, 1);
                cnt++;
            }
        }
        for (y=0; y<h-1; y+=t) {
            progress_meter((int)round(cnt*100.0/tot));
            for (x=0; x<w-1; x+=s) {
                mpd(map, x, y, s, t, w, h, d, 0);
                cnt++;
            }
        }
        progress_meter((int)round(cnt*100.0/tot));
        s /= 2;
        t /= 2;
        dd *= mpd_settings.r;
    }
    progress_meter(-1);
}

const hmg_algorithm_t hmg_algo_mpd_info = {
    "Midpoint Displacement",
    algo_mpd,
    params_mpd
};

