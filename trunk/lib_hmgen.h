#ifndef HMG_LIB_HMGEN_H
#define HMG_LIB_HMGEN_H

extern void (*hmg_progress_meter)(char *context, int p);

typedef enum hmg_param_type_e {
    PARAM_INT,
    PARAM_CHAR,
    PARAM_DOUBLE
} hmg_param_type_t;

typedef struct hmg_param_s {
    char *name;
    void *store;
    double min, max;
    hmg_param_type_t type;
} hmg_param_t;

typedef struct hmg_algorithm_s {
    const char *name;
    void (*func)(unsigned char *map, unsigned char *tempmap,
                 unsigned int width, unsigned int height);
    const hmg_param_t *params;
} hmg_algorithm_t;

extern const hmg_algorithm_t *hmg_algorithms[];

int hmg_parse_params(const char *arg, const hmg_param_t *params);

void hmg_normalize(unsigned char *map,
                   unsigned int normmin, unsigned int normmax,
                   unsigned int w, unsigned int h);

void hmg_gaussian_blur(unsigned char *map, unsigned char *tempmap,
                       int blurx, int blury, double blursigma,
                       unsigned int w, unsigned int h);

void hmg_invert(unsigned char *map, unsigned int w, unsigned int h);

extern unsigned char hmg_colormap[256][3];

typedef struct hmg_colormap_settings_s {
        unsigned int scales[8];
            unsigned char colors[8][3];
} hmg_colormap_settings_t;

void hmg_init_colormap(hmg_colormap_settings_t *settings);

void hmg_rng_initgauss(unsigned int seed);
double hmg_rng_gauss(void);

#ifdef __GNUC__
#define HMG_ATTR_UNUSED     __attribute__((unused))
#else
#define HMG_ATTR_UNUSED
#endif

#define ALGO_FF     0       /* Fault Formation */
#define ALGO_MDP    1       /* Midpoint Displacement */
#define ALGO_FORGE  2       /* ppmforge/gforge */
#define ALGO_LAST   3       /* Nothing */

/* Handy shortcut */

#define progress_meter(a) hmg_progress_meter(HMGEN_MODULE, a)

#endif
