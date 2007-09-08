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

#define HMGEN_MODULE "Fault Formation"

static struct ff_settings_s {
    unsigned int seed;
    unsigned char init_value;
    unsigned int number_of_plates;
    unsigned int up_factor;
    unsigned int down_factor;
} ff_settings = { 42, 128, 512, 1, 1 };

static const hmg_param_t params_ff[] = {
    { "seed=", &ff_settings.seed, 0.0, 4294967295.0, PARAM_INT },
    { "init=", &ff_settings.init_value, 0.0, 255.0, PARAM_CHAR },
    { "n=", &ff_settings.number_of_plates, 0.0, 4294967295.0, PARAM_INT },
    { "up=", &ff_settings.up_factor, 0.0, 255.0, PARAM_INT },
    { "down=", &ff_settings.down_factor, 0.0, 255.0, PARAM_INT },
    { NULL, NULL, 0, 0, 0 }
};

static void algo_ff(unsigned char *map, unsigned char *tempmap HMG_ATTR_UNUSED,
                    unsigned int w, unsigned int h) {
    unsigned int x, y, a = 0, b = 0, c = 0, d = 0, side, i;
    int t;
    float r;

    srand(ff_settings.seed);

    for (x=0; x<w; x++)
        for (y=0; y<h; y++)
            map[x+y*w] = ff_settings.init_value;

    for (i=0; i<ff_settings.number_of_plates; i++) {
        progress_meter((int)round(i*100.0/(ff_settings.number_of_plates-1)));
        switch (rand()&3) {
        case 0:
            a = 0;
            b = rand() % h;
            break;
        case 1:
            a = rand() % w;
            b = 0;
            break;
        case 2:
            a = w-1;
            b = rand() % h;
            break;
        case 3:
            a = rand() % w;
            b = h-1;
            break;
        }
        do {
            switch (rand()&3) {
            case 0:
                c = 0;
                d = rand() % h;
                break;
            case 1:
                c = rand() % w;
                d = 0;
                break;
            case 2:
                c = w-1;
                d = rand() % h;
                break;
            case 3:
                c = rand() % w;
                d = h-1;
                break;
            }
        } while (a == c || b == d);

        side = rand()&1;

        for (x=0; x<w; x++) {
            for (y=0; y<h; y++) {
                r = (float) y - b - (((float)d-b)/((float)c-a)) * ((float)x - (float)a);
                r *= side ? -1 : 1;
                if (r>0) {
                    t = (int)map[x+y*w] + ff_settings.up_factor;
                    map[x+y*w] = t>255 ? 255 : t;
                } else {
                    t = (int)map[x+y*w] - ff_settings.down_factor;
                    map[x+y*w] = t<0 ? 0 : t;
                }
            }
        }
    }
    progress_meter(-1);
}

const hmg_algorithm_t hmg_algo_ff_info = {
    "Fault Formation",
    algo_ff,
    params_ff
};

