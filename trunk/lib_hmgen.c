#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "lib_hmgen.h"

extern const hmg_algorithm_t hmg_algo_ff_info;
extern const hmg_algorithm_t hmg_algo_mpd_info;
extern const hmg_algorithm_t hmg_algo_forge_info;

const hmg_algorithm_t *hmg_algorithms[] = {
    &hmg_algo_ff_info,
    &hmg_algo_mpd_info,
    &hmg_algo_forge_info
};

static void default_progress_meter(char *context, int p) {
    if (p<0)
        fprintf(stderr, "\r[done] %s\n", context);
    else
        fprintf(stderr, "\r[%3i%%] %s\r", p, context);
}

void (*hmg_progress_meter)(char *context, int p) = default_progress_meter;

int hmg_parse_params(const char *arg, const hmg_param_t *params) {
    unsigned int n, i;
    const char *p = arg, *t;

    for (n=0; params[n].name; n++)  ;

    while (p!=NULL && *p!='\0') {
        for (i=0; i<n; i++)
            if (!strncmp(p, params[i].name, strlen(params[i].name)))
                break;

        if (i == n) {
            fprintf(stderr, "unknown parameter: %8s...\n", p);
            return 0;
        }

        p = strchr(p, '=');
        assert(*p == '=');
        p++;

        switch (params[i].type) {
        case PARAM_INT: {
            int x;

            x = atoi(p);
            if (x<params[i].min) {
                fprintf(stderr, "%s, value smaller than %i\n", params[i].name,
                                                        (int) params[i].min);
                return 0;
            } else if (x>params[i].max) {
                fprintf(stderr, "%s, value larger than %i\n", params[i].name,
                                                        (int) params[i].max);
                return 0;
            }
            *(int*)params[i].store = x;
            break; }
        case PARAM_CHAR: {
            int x;

            x = atoi(p);
            if (x<params[i].min) {
                fprintf(stderr, "%s, value smaller than %i\n", params[i].name,
                                                        (int) params[i].min);
                return 0;
            } else if (x>params[i].max) {
                fprintf(stderr, "%s, value larger than %i\n", params[i].name,
                                                        (int) params[i].max);
                return 0;
            }
            *(char*)params[i].store = x;
            break; }
        case PARAM_DOUBLE: {
            double d;

            d = atof(p);
            if (d<params[i].min) {
                fprintf(stderr, "%s, value smaller than %0.2f\n",
                                                params[i].name, params[i].min);
                return 0;
            } else if (d>params[i].max) {
                fprintf(stderr, "%s, value larger than %0.2f\n",
                                                params[i].name, params[i].max);
                return 0;
            }
            *(double*)params[i].store = d;
            break; }
        default:
            assert(0);
            break;
        }

        t = strchr(p, ':');
        if (t == NULL)
            break;
        p = t+1;
    }

    return 1;
}
