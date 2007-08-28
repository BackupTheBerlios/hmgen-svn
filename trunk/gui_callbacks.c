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
static unsigned int map_width = 4097, map_height = 4097;
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

static void render_map(GtkWidget *widget) {
    unsigned int x, y, stride;
    unsigned char *p, *m;

    if (map_pixbuf) {
        gdk_threads_enter();
            g_object_unref(map_pixbuf);
        gdk_threads_leave();
    }

    gdk_threads_enter();
        map_pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, map_width, map_height);
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

void on_algorithm_combobox_changed(GtkComboBox *combobox, gpointer user_data) {
    gint active;
    GtkWidget *w;

    algo_combo_set = 1;
    generate_button_sensitivity(combobox);

    w = lookup_widget(GTK_WIDGET(combobox), "algosettings_notebook");
    active = gtk_combo_box_get_active(combobox);

    gtk_notebook_set_current_page(GTK_NOTEBOOK(w), active);
}

void on_size_combobox_changed(GtkComboBox *combobox, gpointer user_data) {
    size_combo_set = 1;
    generate_button_sensitivity(combobox);
}

static void retrieve_settings_ff(GtkWidget *widget, char *buf, int len) {
    GtkWidget *w;
    unsigned int seed, init, nplates, up, down;

    gdk_threads_enter();
        w = lookup_widget(widget, "ff_seed_spinbutton");
        seed = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w));
        w = lookup_widget(widget, "ff_init_spinbutton");
        init = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w));
        w = lookup_widget(widget, "ff_nplates_spinbutton");
        nplates = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w));
        w = lookup_widget(widget, "ff_upfactor_spinbutton");
        up = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w));
        w = lookup_widget(widget, "ff_downfactor_spinbutton");
        down = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w));
    gdk_threads_leave();

    snprintf(buf, len, "seed=%u:init=%u:n=%u:up=%u:down=%u", seed, init,
                                                        nplates, up, down);
}

static void retrieve_settings_mpd(GtkWidget *widget, char *buf, int len) {
    GtkWidget *w;
    unsigned int seed, tl, tr, bl, br, d;
    float r;

    gdk_threads_enter();
        w = lookup_widget(widget, "mpd_seed_spinbutton");
        seed = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w));
        w = lookup_widget(widget, "mpd_tl_spinbutton");
        tl = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w));
        w = lookup_widget(widget, "mpd_tr_spinbutton");
        tr = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w));
        w = lookup_widget(widget, "mpd_bl_spinbutton");
        bl = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w));
        w = lookup_widget(widget, "mpd_br_spinbutton");
        br = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w));
        w = lookup_widget(widget, "mpd_displacement_spinbutton");
        d = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w));
        w = lookup_widget(widget, "mpd_roughness_spinbutton");
        r = gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(w));
    gdk_threads_leave();

    snprintf(buf, len, "seed=%u:tl=%u:tr=%u:bl=%u:br=%u:d=%u:r=%0.2f", seed,
                                                        tl, tr, bl, br, d, r);
}

static void retrieve_settings_forge(GtkWidget *widget, char *buf, int len) {
    GtkWidget *w;
    unsigned int seed;
    float fracdim, powscale;

    gdk_threads_enter();
        w = lookup_widget(widget, "forge_seed_spinbutton");
        seed = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w));
        w = lookup_widget(widget, "forge_fracdim_spinbutton");
        fracdim = gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(w));
        w = lookup_widget(widget, "forge_powscale_spinbutton");
        powscale = gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(w));
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

static void *generate_thread(void *args) {
    GtkWidget *widget;
    gint active;
    unsigned int dims[] = { 3, 5, 9, 17, 33, 65, 129, 257, 513, 1025, 2049, 4097, 8193, 16385 };
    unsigned int normmin, normmax, normfirst, normlast, inv, blurx, blury;
    unsigned int blur;
    double blursigma;
    char *settings;
 
    if (!main_progressbar) {
        gdk_threads_enter();
            widget = lookup_widget(GTK_WIDGET(args), "main_progressbar");
            main_progressbar = GTK_PROGRESS_BAR(widget);
        gdk_threads_leave();
    }
 
    hmg_progress_meter = gui_progress_meter;
    gdk_threads_enter();
        widget = lookup_widget(GTK_WIDGET(args), "size_combobox");
        active = gtk_combo_box_get_active(GTK_COMBO_BOX(widget));
    gdk_threads_leave();
    map_width = map_height = dims[active];

    if (map_pixbuf) {
        gdk_threads_enter();
            g_object_unref(map_pixbuf);
            map_pixbuf = NULL;
        gdk_threads_leave();
    }

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
        widget = lookup_widget(GTK_WIDGET(args), "norm_min_spinbutton");
        normmin = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
        widget = lookup_widget(GTK_WIDGET(args), "norm_max_spinbutton");
        normmax = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
        widget = lookup_widget(GTK_WIDGET(args), "invert_checkbutton");
        inv = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
        widget = lookup_widget(GTK_WIDGET(args), "blur_checkbutton");
        blur = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
        widget = lookup_widget(GTK_WIDGET(args), "blur_xradius_spinbutton");
        blurx = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
        widget = lookup_widget(GTK_WIDGET(args), "blur_yradius_spinbutton");
        blury = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
        widget = lookup_widget(GTK_WIDGET(args), "blur_sigma_spinbutton");
        blursigma = gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(widget));
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

    if (tempmap)
        free(tempmap);
    tempmap = NULL;

    render_map(args);

stopit:
    gdk_threads_enter();
        widget = lookup_widget(GTK_WIDGET(args), "main_notebook");
        gtk_widget_set_sensitive (widget, TRUE);
        gtk_notebook_set_current_page(GTK_NOTEBOOK(widget), 0);
    gdk_threads_leave();

    return NULL;
}

void on_generate_button_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *widget = lookup_widget(GTK_WIDGET(button), "main_notebook");

    gtk_widget_set_sensitive (widget, FALSE);

    g_thread_create(generate_thread, button, FALSE, NULL);
}

void on_pgm_save_as_button_clicked(GtkButton *button, gpointer user_data) {
}


void on_ppm_save_as_button_clicked(GtkButton *button, gpointer user_data) {
}

gboolean on_view_drawingarea_expose_event(GtkWidget *widget,
                                          GdkEventExpose *event,
                                          gpointer user_data) {
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

void gui_quit(GtkWidget *widget, gpointer user_data) {
    if (map_pixbuf)
        g_object_unref(map_pixbuf);
    if (map)
        free(map);
    if (tempmap)
        free(tempmap);

    gtk_main_quit();
}

void on_new_create_button_clicked(GtkButton *button, gpointer user_data) {
}

void on_norm2_button_clicked(GtkButton *button, gpointer user_data) {
}

void on_blur2_button_clicked(GtkButton *button, gpointer user_data) {
}

void on_invert2_button_clicked(GtkButton *button, gpointer user_data) {
}

gboolean on_colormap_display_expose_event(GtkWidget *widget,
                                          GdkEventExpose *event,
                                          gpointer user_data) {
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

