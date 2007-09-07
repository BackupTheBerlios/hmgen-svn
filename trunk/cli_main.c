#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "lib_hmgen.h"
#include "version.h"

#define DEF_BLURX       31
#define DEF_BLURY       31
#define DEF_BLURSIGMA   6.0f

#define DEF_NORMMIN     0
#define DEF_NORMMAX     255

#define DEF_CLIPMIN     96
#define DEF_CLIPMAX     160

static int size = 8, algo = 0, blur = 0, blurx = DEF_BLURX, blury = DEF_BLURY;
static double blursigma = DEF_BLURSIGMA;
static int normmin = DEF_NORMMIN, normmax = DEF_NORMMAX,
           normfirst = 0, normlast = 0, inv = 0, clip = 0,
           clipmin = DEF_CLIPMIN, clipmax = DEF_CLIPMAX,
           crop = 0, cropleft = 0, cropright = 0, croptop = 0, cropbottom = 0;
static unsigned int w, h;
static char *outfile = NULL;
static char *colfile = NULL;
static char *bmpfile = NULL;
static const char *params = NULL;

static unsigned char *map, *tempmap;

static void help_message(char **argv) {
    fprintf(stderr,
"\nHeight Map Generator - %s - %s\n\n"
"usage: %s [options]\n\n"
"-s INT             size, 2^VAL+1 [default: 8]\n"
"-o FILE            output heightmap to PGM file\n"
"-ppm FILE          output colorized version to PPM file\n"
"-bmp FILE          output map to BMP file\n"
"-p STRING          algorithm parameters (param:param:param...)\n"
"-a INT             algorithm [default: 0]\n"
"                       0 - Fault Formation\n"
"                           seed=INT        random seed\n"
"                           init=INT        initial height (0-255)\n"
"                           n=INT           number of tectonic plates\n"
"                           up=INT          move up factor\n"
"                           down=INT        move down factor\n"
"                       1 - Midpoint Displacement (plasma)\n"
"                           seed=INT        random seed\n"
"                           tl=INT          top-left initial height\n"
"                           tr=INT          top-right initial height\n"
"                           bl=INT          bottom-left initial height\n"
"                           br=INT          bottom-right initial height\n"
"                           d=INT           initial displacement\n"
"                           r=FLOAT         roughness factor\n"
"                       2 - ppmforge/gforge Algorithm\n"
"                           seed=INT        random seed\n"
"                           fracdim=FLOAT   fractal dimension\n"
"                           powscale=FLOAT  power scale\n"
"\n"
"-nf                postprocess, normalize first [default: no]\n"
"-nl                postprocess, normalize last [default: no]\n"
"-nmin INT          normalize minimum (0-255) [default: %i]\n"
"-nmax INT          normalize maximum (0-255) [default: %i]\n"
"-b                 postprocess, gaussian blur [default: no]\n"
"-bx INT            blur x-radius [default: %i]\n"
"-by INT            blur y-radius [default: %i]\n"
"-bxy INT           blur xy-radius\n"
"-bs FLOAT          blur sigma [default: %0.2f]\n"
"-i                 postprocess, invert heightmap [default: no]\n"
"-clip              postprocess, clip [default: no]\n"
"-clipmin INT       clip minimum (0-255) [default: %i]\n"
"-clipmax INT       clip maximum (0-255) [default: %i]\n"
"-crop              postprocess, crop [default: no]\n"
"-cropleft INT      crop left margin (0-..) [default: 0]\n"
"-cropright INT     crop right margin (0-..) [default: 0]\n"
"-croptop INT       crop top margin (0-..) [default: 0]\n"
"-cropbottom INT    crop bottom margin (0-..) [default: 0]\n"
"\n",
        HMG_VERSION_STRING, HMG_COPYRIGHT_STRING, argv[0],
        DEF_NORMMIN, DEF_NORMMAX,
        DEF_BLURX, DEF_BLURY, DEF_BLURSIGMA,
        DEF_CLIPMIN, DEF_CLIPMAX
            );
    exit(0);
}

static inline int test_argument(int a, int argc, char **argv) {
    if (a+1 >= argc) {
        fprintf(stderr, "option %s needs an argument\n", argv[a]);
        return 0;
    } else
        return 1;
}

static int parse_command_line(int argc, char**argv) {
    int a;

    a = 1;

    while (a < argc) {
        if (!strcmp(argv[a], "-s")) {
            if (!test_argument(a, argc, argv)) return 0;
            size = atoi(argv[++a]);
        } else if (!strcmp(argv[a], "-a")) {
            if (!test_argument(a, argc, argv)) return 0;
            algo = atoi(argv[++a]);
        } else if (!strcmp(argv[a], "-o")) {
            if (!test_argument(a, argc, argv)) return 0;
            outfile = argv[++a];
        } else if (!strcmp(argv[a], "-ppm")) {
            if (!test_argument(a, argc, argv)) return 0;
            colfile = argv[++a];
        } else if (!strcmp(argv[a], "-bmp")) {
            if (!test_argument(a, argc, argv)) return 0;
            bmpfile = argv[++a];
        } else if (!strcmp(argv[a], "-p")) {
            if (!test_argument(a, argc, argv)) return 0;
            params = argv[++a];
        } else if (!strcmp(argv[a], "-b")) {
            blur = 1;
        } else if (!strcmp(argv[a], "-bx")) {
            if (!test_argument(a, argc, argv)) return 0;
            blurx = atoi(argv[++a]);
        } else if (!strcmp(argv[a], "-by")) {
            if (!test_argument(a, argc, argv)) return 0;
            blury = atoi(argv[++a]);
        } else if (!strcmp(argv[a], "-bxy")) {
            if (!test_argument(a, argc, argv)) return 0;
            blurx = blury = atoi(argv[++a]);
        } else if (!strcmp(argv[a], "-bs")) {
            if (!test_argument(a, argc, argv)) return 0;
            blursigma = atof(argv[++a]);
        } else if (!strcmp(argv[a], "-h")) {
            help_message(argv);
        } else if (!strcmp(argv[a], "-nf")) {
            normfirst = 1;
        } else if (!strcmp(argv[a], "-nl")) {
            normlast = 1;
        } else if (!strcmp(argv[a], "-nmin")) {
            if (!test_argument(a, argc, argv)) return 0;
            normmin = atoi(argv[++a]);
        } else if (!strcmp(argv[a], "-nmax")) {
            if (!test_argument(a, argc, argv)) return 0;
            normmax = atoi(argv[++a]);
        } else if (!strcmp(argv[a], "-i")) {
            inv = 1;
        } else if (!strcmp(argv[a], "-clip")) {
            clip = 1;
        } else if (!strcmp(argv[a], "-clipmin")) {
            if (!test_argument(a, argc, argv)) return 0;
            clipmin = atoi(argv[++a]);
        } else if (!strcmp(argv[a], "-clipmax")) {
            if (!test_argument(a, argc, argv)) return 0;
            clipmax = atoi(argv[++a]);
        } else if (!strcmp(argv[a], "-crop")) {
            crop = 1;
        } else if (!strcmp(argv[a], "-cropleft")) {
            if (!test_argument(a, argc, argv)) return 0;
            cropleft = atoi(argv[++a]);
        } else if (!strcmp(argv[a], "-cropright")) {
            if (!test_argument(a, argc, argv)) return 0;
            cropright = atoi(argv[++a]);
        } else if (!strcmp(argv[a], "-croptop")) {
            if (!test_argument(a, argc, argv)) return 0;
            croptop = atoi(argv[++a]);
        } else if (!strcmp(argv[a], "-cropbottom")) {
            if (!test_argument(a, argc, argv)) return 0;
            cropbottom = atoi(argv[++a]);
        } else {
            fprintf(stderr, "unknown option: %s\n", argv[a]);
            return 0;
        }
        a++;
    }

    return 1;
}

static int test_global_settings(void) {
    if (size<1 || size>14) {
        fprintf(stderr, "size is smaller than 1 or larger than 16\n");
        return 0;
    }
    w = h = exp2(size) + 1;
    if (algo<ALGO_FF || algo>=ALGO_LAST) {
        fprintf(stderr, "algo is smaller then %i or larger than %i\n",
                ALGO_FF, ALGO_LAST-1);
        return 0;
    }
    if (blurx < 1) {
        fprintf(stderr, "blurx is smaller than 1\n");
        return 0;
    }
    if (blury < 1) {
        fprintf(stderr, "blury is smaller than 1\n");
        return 0;
    }
    if (normmin < 0) {
        fprintf(stderr, "normmin is smaller than 0\n");
        return 0;
    }
    if (normmax > 255) {
        fprintf(stderr, "normmax is larger than 255\n");
        return 0;
    }
    if (clipmin < 0) {
        fprintf(stderr, "clipmin is smaller than 0\n");
        return 0;
    }
    if (clipmax > 255) {
        fprintf(stderr, "clipmax is larger than 255\n");
        return 0;
    }
    if ((croptop + cropbottom + 3U) > h) {
        fprintf(stderr, "vertical cropping beyond width\n");
        return 0;
    }
    if ((cropleft + cropright + 3U) > w) {
        fprintf(stderr, "horizontal cropping beyond width\n");
        return 0;
    }
    return 1;
}

int main(int argc, char **argv) {
    if (!parse_command_line(argc, argv)) {
        fprintf(stderr, "error, parsing command line\n");
        exit(1);
    }
    if (!test_global_settings()) {
        fprintf(stderr, "error, invalid global settings\n");
        exit(1);
    }

    w = h = exp2(size) + 1;

    fprintf(stderr, "Dimensions: %i x %i\n", w, h);
    fprintf(stderr, "Algorithm: %s\n", hmg_algorithms[algo]->name);

    if (!hmg_parse_params(params, hmg_algorithms[algo]->params)) {
        fprintf(stderr, "error, parsing parameters\n");
        exit(1);
    }

    map = calloc(h, w);
    if (!map) {
        fprintf(stderr, "error, unable to allocate memory\n");
        exit(1);
    }
    tempmap = calloc(h, w);
    if (!tempmap) {
        fprintf(stderr, "error, unable to allocate memory\n");
        free(map);
        exit(1);
    }

    hmg_algorithms[algo]->func(map, tempmap, w, h);

    if (crop)
        hmg_crop(&map, &w, &h, cropleft, cropright, croptop, cropbottom);
    if (normfirst)
        hmg_normalize(map, normmin, normmax, w, h);
    if (blur)
        hmg_gaussian_blur(map, tempmap, blurx, blury, blursigma, w, h);
    if (inv)
        hmg_invert(map, w, h);
    if (clip)
        hmg_clip(map, clipmin, clipmax, w, h);
    if (normlast)
        hmg_normalize(map, normmin, normmax, w, h);

    if (outfile) {
        fprintf(stderr, "Saving to %s\n", outfile);
        if (!hmg_export_pgm(outfile, map, w, h))
            fprintf(stderr, "error during saving\n");
    }

    if (colfile) {
        fprintf(stderr, "Saving PPM file to %s\n", colfile);
        hmg_init_colormap(NULL);
        if (!hmg_export_ppm(colfile, map, w, h))
            fprintf(stderr, "error during saving\n");
    }

    if (bmpfile) {
        fprintf(stderr, "Saving BMP file to %s\n", bmpfile);
        hmg_init_colormap(NULL);
        if (!hmg_export_bmp(bmpfile, map, w, h))
            fprintf(stderr, "error during saving\n");
    }

    free(tempmap);
    free(map);
    return 0;
}
