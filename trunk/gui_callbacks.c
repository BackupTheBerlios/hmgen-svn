#include <gtk/gtk.h>
#include <string.h>
#include <assert.h>
#include <glib.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include "gui_callbacks.h"
#include "gui_interface.h"
#include "gui_support.h"
#include "lib_hmgen.h"

static unsigned char *map = NULL, *tempmap = NULL;
static GdkPixbuf *map_pixbuf = NULL;
static unsigned int map_width = 0, map_height = 0;
static GtkProgressBar *main_progressbar = NULL;
static unsigned int algo_combo_set = 0, size_combo_set = 0;

static void gui_progress_meter(char *context, int p) {
    static char *prevcontext = NULL;
    static int prevp = 0;

    if (p>=0) {
        gdk_threads_enter();
            if (context != prevcontext) {
                gtk_progress_bar_set_text(main_progressbar, context);
                prevcontext = context;
            }
            if (p>prevp) {
                gtk_progress_bar_set_fraction(main_progressbar, p/100.0);
                prevp = p;
            }
        gdk_threads_leave();
    } else {
        gdk_threads_enter();
            gtk_progress_bar_set_text(main_progressbar, "Idle...");
            gtk_progress_bar_set_fraction(main_progressbar, 0.0);
            prevp = 0;
            prevcontext = NULL;
        gdk_threads_leave();
    }
}

static void unref_map_pixbuf(void) {
    if (map_pixbuf) {
        gdk_threads_enter();
            g_object_unref(map_pixbuf);
        gdk_threads_leave();
        map_pixbuf = NULL;
    }
}

static void render_map(GtkWidget *widget HMG_ATTR_UNUSED) {
    unsigned int x, y, stride;
    unsigned char *p, *m;

    if (!map_width || !map_height)
        return;

    unref_map_pixbuf();

    gdk_threads_enter();
        map_pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, map_width,
                                                                map_height);
        stride = gdk_pixbuf_get_rowstride(map_pixbuf);
        p = gdk_pixbuf_get_pixels(map_pixbuf);
    gdk_threads_leave();

    m = map;

    for (y=0; y<map_height; y++) {
        if (y&0x10)
            gui_progress_meter("Rendering", round(y*100.0/(map_height-1)));
        for (x=0; x<map_width; x++) {
            int x3 = x + (x<<1);
            p[x3  ] = hmg_colormap[m[x]][0];
            p[x3+1] = hmg_colormap[m[x]][1];
            p[x3+2] = hmg_colormap[m[x]][2];
        }
        m += map_width;
        p += stride;
    }
    gui_progress_meter("", -1);
}

static void generate_button_sensitivity(GtkComboBox *cb) {
    GtkWidget *w;

    if (algo_combo_set && size_combo_set) {
        w = lookup_widget(GTK_WIDGET(cb), "generate_button");
        gtk_widget_set_sensitive (w, TRUE);
    }
}

void on_algorithm_combobox_changed(GtkComboBox *combobox,
                                   gpointer user_data HMG_ATTR_UNUSED) {
    gint active;
    GtkWidget *w;

    algo_combo_set = 1;
    generate_button_sensitivity(combobox);

    w = lookup_widget(GTK_WIDGET(combobox), "algosettings_notebook");
    active = gtk_combo_box_get_active(combobox);

    gtk_notebook_set_current_page(GTK_NOTEBOOK(w), active);
}

void on_size_combobox_changed(GtkComboBox *combobox,
                              gpointer user_data HMG_ATTR_UNUSED) {
    size_combo_set = 1;
    generate_button_sensitivity(combobox);
}

/* not thread safe; wrap it yourself */
static int get_spin_button_int(void *widget, const char *name) {
    GtkWidget *w;
    w = lookup_widget(GTK_WIDGET(widget), name);
    return gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w));
}
static float get_spin_button_float(void *widget, const char *name) {
    GtkWidget *w;
    w = lookup_widget(GTK_WIDGET(widget), name);
    return gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(w));
}

static void retrieve_settings_ff(GtkWidget *widget, char *buf, int len) {
    unsigned int seed, init, nplates, up, down;

    gdk_threads_enter();
        seed    = get_spin_button_int(widget, "ff_seed_spinbutton");
        init    = get_spin_button_int(widget, "ff_init_spinbutton");
        nplates = get_spin_button_int(widget, "ff_nplates_spinbutton");
        up      = get_spin_button_int(widget, "ff_upfactor_spinbutton");
        down    = get_spin_button_int(widget, "ff_downfactor_spinbutton");
    gdk_threads_leave();

    snprintf(buf, len, "seed=%u:init=%u:n=%u:up=%u:down=%u", seed, init,
                                                        nplates, up, down);
}

static void retrieve_settings_mpd(GtkWidget *widget, char *buf, int len) {
    unsigned int seed, tl, tr, bl, br, d;
    float r;

    gdk_threads_enter();
        seed = get_spin_button_int  (widget, "mpd_seed_spinbutton");
        tl   = get_spin_button_int  (widget, "mpd_tl_spinbutton");
        tr   = get_spin_button_int  (widget, "mpd_tr_spinbutton");
        bl   = get_spin_button_int  (widget, "mpd_bl_spinbutton");
        br   = get_spin_button_int  (widget, "mpd_br_spinbutton");
        d    = get_spin_button_int  (widget, "mpd_displacement_spinbutton");
        r    = get_spin_button_float(widget, "mpd_roughness_spinbutton");
    gdk_threads_leave();

    snprintf(buf, len, "seed=%u:tl=%u:tr=%u:bl=%u:br=%u:d=%u:r=%0.2f", seed,
                                                        tl, tr, bl, br, d, r);
}

static void retrieve_settings_forge(GtkWidget *widget, char *buf, int len) {
    unsigned int seed;
    float fracdim, powscale;

    gdk_threads_enter();
        seed     = get_spin_button_int  (widget, "forge_seed_spinbutton");
        fracdim  = get_spin_button_float(widget, "forge_fracdim_spinbutton");
        powscale = get_spin_button_float(widget, "forge_powscale_spinbutton");
    gdk_threads_leave();

    snprintf(buf, len, "seed=%u:fracdim=%0.2f:powscale=%0.2f", seed, fracdim,
                                                                powscale);
}

#define BUFSIZE 512
static char *retrieve_settings(GtkWidget *widget, int which) {
    static char settings[BUFSIZE];
    void (*retrieve[])(GtkWidget *widget, char *buf, int len) = {
        retrieve_settings_ff,
        retrieve_settings_mpd,
        retrieve_settings_forge
    };
    retrieve[which](widget, settings, BUFSIZE);
    return settings;
}

static void set_main_progressbar(void *args) {
    GtkWidget *widget;

    if (!main_progressbar) {
        gdk_threads_enter();
            widget = lookup_widget(GTK_WIDGET(args), "main_progressbar");
            main_progressbar = GTK_PROGRESS_BAR(widget);
        gdk_threads_leave();
    }
}
 
static void activate_main_notebook(void *args) {
    GtkWidget *widget;

    gdk_threads_enter();
        widget = lookup_widget(GTK_WIDGET(args), "main_notebook");
        gtk_widget_set_sensitive (widget, TRUE);
        gtk_notebook_set_current_page(GTK_NOTEBOOK(widget), 0);
    gdk_threads_leave();
}

static void *generate_thread(void *args) {
    GtkWidget *widget;
    gint active;
    unsigned int dims[] = { 3, 5, 9, 17, 33, 65, 129, 257, 513, 1025, 2049, 4097, 8193, 16385 };
    unsigned int normmin, normmax, normfirst, normlast, inv, blurx, blury;
    unsigned int blur;
    double blursigma;
    char *settings;
 
    set_main_progressbar(args);

    hmg_progress_meter = gui_progress_meter;

    gdk_threads_enter();
        widget = lookup_widget(GTK_WIDGET(args), "size_combobox");
        active = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
    gdk_threads_leave();
    map_width = map_height = dims[active];

    unref_map_pixbuf();

    map     = realloc(map,     map_width * map_height);
    tempmap = realloc(tempmap, map_width * map_height);

    if (!map || !tempmap) {
        gui_progress_meter("Out of memory!", 42);
        goto stopit;
    }

    gdk_threads_enter();
        widget = lookup_widget(GTK_WIDGET(args), "algorithm_combobox");
        active = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
    gdk_threads_leave();

    settings = retrieve_settings(args, active);
    hmg_parse_params(settings, hmg_algorithms[active]->params);
    hmg_algorithms[active]->func(map, tempmap, map_width, map_height);

    // post-processing

    gdk_threads_enter();
        widget = lookup_widget(GTK_WIDGET(args), "norm_first_checkbutton");
        normfirst = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
        widget = lookup_widget(GTK_WIDGET(args), "norm_last_checkbutton");
        normlast = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
        normmin = get_spin_button_int(args, "norm_min_spinbutton");
        normmax = get_spin_button_int(args, "norm_max_spinbutton");
        widget = lookup_widget(GTK_WIDGET(args), "invert_checkbutton");
        inv = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
        widget = lookup_widget(GTK_WIDGET(args), "blur_checkbutton");
        blur = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
        blurx     = get_spin_button_int  (args, "blur_xradius_spinbutton");
        blury     = get_spin_button_int  (args, "blur_yradius_spinbutton");
        blursigma = get_spin_button_float(args, "blur_sigma_spinbutton");
    gdk_threads_leave();

    if (normfirst)
        hmg_normalize(map, normmin, normmax, map_width, map_height);

    if (blur)
        hmg_gaussian_blur(map, tempmap, blurx, blury, blursigma, map_width,
                                                                map_height);
    if (normlast)
        hmg_normalize(map, normmin, normmax, map_width, map_height);

    if (inv)
        hmg_invert(map, map_width, map_height);

/* hmmm, some "sidebar" commands may need tempmap, like gaussian blur
 * FIXME increases memory requirements
    if (tempmap)
        free(tempmap);
    tempmap = NULL;
*/

    render_map(args);

stopit:
    activate_main_notebook(args);
    return NULL;
}

static void deactivate_main_notebook(void *args) {
    GtkWidget *widget = lookup_widget(GTK_WIDGET(args), "main_notebook");

    gtk_widget_set_sensitive (widget, FALSE);
}

void on_generate_button_clicked(GtkButton *button,
                                gpointer user_data HMG_ATTR_UNUSED) {
    deactivate_main_notebook(button);
    g_thread_create(generate_thread, button, FALSE, NULL);
}

void on_pgm_save_as_button_clicked(GtkButton *button,
                                   gpointer user_data HMG_ATTR_UNUSED) {
}


void on_ppm_save_as_button_clicked(GtkButton *button,
                                   gpointer user_data HMG_ATTR_UNUSED) {
}

gboolean on_view_drawingarea_expose_event(GtkWidget *widget,
                                          GdkEventExpose *event HMG_ATTR_UNUSED,
                                          gpointer user_data HMG_ATTR_UNUSED) {
    GdkDrawable *drawable = widget->window;
    GdkGC *gc = widget->style->fg_gc[GTK_WIDGET_STATE (widget)];
    gint width, height;

    if (!map_pixbuf)
        return FALSE;

    width = gdk_pixbuf_get_width(map_pixbuf);
    height = gdk_pixbuf_get_height(map_pixbuf);

    gtk_widget_set_size_request(widget, width, height);

    gdk_draw_pixbuf(drawable, gc, map_pixbuf, 0, 0, 0, 0, width, height,
            GDK_RGB_DITHER_NONE, 0, 0);

    return FALSE;
}

void gui_quit(GtkWidget *widget HMG_ATTR_UNUSED,
              gpointer user_data HMG_ATTR_UNUSED) {
    if (map_pixbuf)
        g_object_unref(map_pixbuf);
    if (map)
        free(map);
    if (tempmap)
        free(tempmap);

    gtk_main_quit();
}

static void *new_create_thread(void *args) {
    unsigned int level;

    set_main_progressbar(args);

    hmg_progress_meter = gui_progress_meter;

    gdk_threads_enter();
        map_width  = get_spin_button_int(args, "new_width_spinbutton");
        map_height = get_spin_button_int(args, "new_height_spinbutton");
        level      = get_spin_button_int(args, "new_level_spinbutton");
    gdk_threads_leave();

    unref_map_pixbuf();

    map     = realloc(map,     map_width * map_height);
    tempmap = realloc(tempmap, map_width * map_height);

    if (!map || !tempmap) {
        gui_progress_meter("Out of memory!", 42);
    } else {
        memset(map, level, map_width * map_height);
        render_map(args);
    }

    activate_main_notebook(args);
    return NULL;
}

static void *invert2_thread(void *args) {
    set_main_progressbar(args);
    hmg_progress_meter = gui_progress_meter;

    hmg_invert(map, map_width, map_height);
    render_map(args);

    activate_main_notebook(args);
    return NULL;
}

static void *norm2_thread(void *args) {
    unsigned int normmin, normmax;

    set_main_progressbar(args);
    hmg_progress_meter = gui_progress_meter;

    gdk_threads_enter();
        normmin = get_spin_button_int  (args, "norm2_min_spinbutton");
        normmax = get_spin_button_int  (args, "norm2_max_spinbutton");
    gdk_threads_leave();

    hmg_normalize(map, normmin, normmax, map_width, map_height);
    render_map(args);

    activate_main_notebook(args);
    return NULL;
}

static void *blur2_thread(void *args) {
    unsigned int blurx, blury;
    double blursigma;

    set_main_progressbar(args);
    hmg_progress_meter = gui_progress_meter;

    gdk_threads_enter();
        blurx     = get_spin_button_int  (args, "blur2_xrad_spinbutton");
        blury     = get_spin_button_int  (args, "blur2_yrad_spinbutton");
        blursigma = get_spin_button_float(args, "blur2_sigma_spinbutton");
    gdk_threads_leave();

    hmg_gaussian_blur(map, tempmap, blurx, blury, blursigma, map_width,
                                                                map_height);
    render_map(args);

    activate_main_notebook(args);
    return NULL;
}

void on_new_create_button_clicked(GtkButton *button,
                                  gpointer user_data HMG_ATTR_UNUSED) {
    deactivate_main_notebook(button);
    g_thread_create(new_create_thread, button, FALSE, NULL);
}

void on_norm2_button_clicked(GtkButton *button,
                             gpointer user_data HMG_ATTR_UNUSED) {
    deactivate_main_notebook(button);
    g_thread_create(norm2_thread, button, FALSE, NULL);
}

void on_blur2_button_clicked(GtkButton *button,
                             gpointer user_data HMG_ATTR_UNUSED) {
    deactivate_main_notebook(button);
    g_thread_create(blur2_thread, button, FALSE, NULL);
}

void on_invert2_button_clicked(GtkButton *button,
                               gpointer user_data HMG_ATTR_UNUSED) {
    deactivate_main_notebook(button);
    g_thread_create(invert2_thread, button, FALSE, NULL);
}

gboolean on_colormap_display_expose_event(GtkWidget *widget,
                                          GdkEventExpose *event HMG_ATTR_UNUSED,
                                          gpointer user_data HMG_ATTR_UNUSED) {
    GdkDrawable *drawable = widget->window;
    GdkGC *gc = gdk_gc_new(drawable);
    unsigned int width = 514, height = 32, x;
    GdkColor ourcolor = { 0, 0, 0, 0 };

    gtk_widget_set_size_request(widget, width, height);

    gdk_gc_set_foreground(gc, &ourcolor);
    gdk_draw_rectangle(drawable, gc, TRUE, 0, 0, width, height);

    for (x=0; x<256; x++) {
        ourcolor.pixel = (hmg_colormap[x][0]<<16) +
                         (hmg_colormap[x][1]<<8)  +
                          hmg_colormap[x][2];
        gdk_gc_set_foreground(gc, &ourcolor);
        gdk_draw_rectangle(drawable, gc, FALSE, 1+2*x, 1, 1, height-3);
    }

    g_object_unref(gc);
    return FALSE;
}

