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
static unsigned int colormap_auto_update = 0;

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

void on_algorithm_combobox_changed(GtkComboBox *combobox,
                                   gpointer user_data HMG_ATTR_UNUSED) {
    gint active;
    GtkWidget *w;

    w = lookup_widget(GTK_WIDGET(combobox), "algosettings_notebook");
    active = gtk_combo_box_get_active(combobox);

    gtk_notebook_set_current_page(GTK_NOTEBOOK(w), active);
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
static int get_check_button_active(void *widget, const char *name) {
    GtkWidget *w;
    w = lookup_widget(GTK_WIDGET(widget), name);
    return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w));
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
 
static void activate_save_buttons(void *args) {
    GtkWidget *widget;

    gdk_threads_enter();
        widget = lookup_widget(GTK_WIDGET(args), "export_save_button");
        gtk_widget_set_sensitive (widget, TRUE);
    gdk_threads_leave();
}

static void activate_main_notebook(void *args, int to_2dview) {
    GtkWidget *widget;

    gdk_threads_enter();
        widget = lookup_widget(GTK_WIDGET(args), "main_notebook");
        gtk_widget_set_sensitive (widget, TRUE);
        if (to_2dview)
        gtk_notebook_set_current_page(GTK_NOTEBOOK(widget), 0);
    gdk_threads_leave();
}

static void sanitize_crop_settings(unsigned int *cropleft,
                                   unsigned int *cropright,
                                   unsigned int *croptop,
                                   unsigned int *cropbottom) {
    /* sanitize crop settings if necessary */
    if (*cropleft > map_width-3)
        *cropleft = map_width-3;
    if (*cropright > map_width-3)
        *cropright = map_width-3;
    if ((*cropright + *cropleft) > map_width-3) {
        *cropright = 0;
        *cropleft = map_width-3;
    }
    if (*croptop > map_height-3)
        *croptop = map_height-3;
    if (*cropbottom > map_height-3)
        *cropbottom = map_height-3;
    if ((*cropbottom + *croptop) > map_height-3) {
        *cropbottom = 0;
        *croptop = map_height-3;
    }
}

static void *generate_thread(void *args) {
    GtkWidget *widget;
    gint active;
    unsigned int dims[] = { 3, 5, 9, 17, 33, 65, 129, 257, 513, 1025, 2049, 4097, 8193, 16385 };
    unsigned int normmin, normmax, normfirst, normlast, inv, blurx, blury;
    unsigned int blur, crop, cropleft, cropright, croptop, cropbottom;
    unsigned int clip, clipmin, clipmax;
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
        normfirst   = get_check_button_active(args, "norm_first_checkbutton");
        normlast    = get_check_button_active(args, "norm_last_checkbutton");
        normmin     = get_spin_button_int    (args, "norm_min_spinbutton");
        normmax     = get_spin_button_int    (args, "norm_max_spinbutton");
        inv         = get_check_button_active(args, "invert_checkbutton");
        blur        = get_check_button_active(args, "blur_checkbutton");
        blurx       = get_spin_button_int    (args, "blur_xradius_spinbutton");
        blury       = get_spin_button_int    (args, "blur_yradius_spinbutton");
        blursigma   = get_spin_button_float  (args, "blur_sigma_spinbutton");
        crop        = get_check_button_active(args, "crop_enable_checkbutton");
        cropleft    = get_spin_button_int    (args, "crop_left_spinbutton");
        cropright   = get_spin_button_int    (args, "crop_right_spinbutton");
        croptop     = get_spin_button_int    (args, "crop_top_spinbutton");
        cropbottom  = get_spin_button_int    (args, "crop_bottom_spinbutton");
        clip        = get_check_button_active(args, "clip_enable_checkbutton");
        clipmin     = get_spin_button_int    (args, "clip_min_spinbutton");
        clipmax     = get_spin_button_int    (args, "clip_max_spinbutton");
    gdk_threads_leave();

    if (crop) {
        sanitize_crop_settings(&cropleft, &cropright, &croptop, &cropbottom);
        hmg_crop(&map, &map_width, &map_height, cropleft, cropright,
                croptop, cropbottom);
    }
    if (normfirst)
        hmg_normalize(map, normmin, normmax, map_width, map_height);
    if (blur)
        hmg_gaussian_blur(map, tempmap, blurx, blury, blursigma, map_width,
                                                                map_height);
    if (inv)
        hmg_invert(map, map_width, map_height);
    if (clip)
        hmg_clip(map, clipmin, clipmax, map_width, map_height);
    if (normlast)
        hmg_normalize(map, normmin, normmax, map_width, map_height);

/* hmmm, some "sidebar" commands may need tempmap, like gaussian blur
 * FIXME increases memory requirements
    if (tempmap)
        free(tempmap);
    tempmap = NULL;
*/

    render_map(args);

    activate_save_buttons(args);

stopit:
    activate_main_notebook(args, 1);
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

    activate_save_buttons(args);
    activate_main_notebook(args, 1);
    return NULL;
}

static void *invert2_thread(void *args) {
    set_main_progressbar(args);
    hmg_progress_meter = gui_progress_meter;

    hmg_invert(map, map_width, map_height);
    render_map(args);

    activate_main_notebook(args, 1);
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

    activate_main_notebook(args, 1);
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

    activate_main_notebook(args, 1);
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

static void adjust_other_scales(GtkRange *range, int curscale, double val) {
    double v;
    GtkWidget *w;
    int c;
    char name[] = "colormap_scalex";
    char nums[] = "0123456789";

    for (c=curscale-1; c>0; c--) {
        name[14] = nums[c];
        w = lookup_widget(GTK_WIDGET(range), name);
        v = gtk_range_get_value(GTK_RANGE(w));
        if (v>val)
            gtk_range_set_value(GTK_RANGE(w), val);
    }
    for (c=curscale+1; c<9; c++) {
        name[14] = nums[c];
        w = lookup_widget(GTK_WIDGET(range), name);
        v = gtk_range_get_value(GTK_RANGE(w));
        if (v<val)
            gtk_range_set_value(GTK_RANGE(w), val);
    }

    /* the button is not used as a button inside the update function, so we
     * can safely convert our range to a button */
    if (colormap_auto_update)
        on_colormap_update_button_clicked(GTK_BUTTON(range), NULL);
}

void on_colormap_scale1_value_changed(GtkRange *range,
                                      gpointer user_data HMG_ATTR_UNUSED) {
    adjust_other_scales(range, 1, gtk_range_get_value(range));
}

void on_colormap_scale2_value_changed(GtkRange *range,
                                      gpointer user_data HMG_ATTR_UNUSED) {
    adjust_other_scales(range, 2, gtk_range_get_value(range));
}

void on_colormap_scale3_value_changed(GtkRange *range,
                                      gpointer user_data HMG_ATTR_UNUSED) {
    adjust_other_scales(range, 3, gtk_range_get_value(range));
}

void on_colormap_scale4_value_changed(GtkRange *range,
                                      gpointer user_data HMG_ATTR_UNUSED) {
    adjust_other_scales(range, 4, gtk_range_get_value(range));
}

void on_colormap_scale5_value_changed(GtkRange *range,
                                      gpointer user_data HMG_ATTR_UNUSED) {
    adjust_other_scales(range, 5, gtk_range_get_value(range));
}

void on_colormap_scale6_value_changed(GtkRange *range,
                                      gpointer user_data HMG_ATTR_UNUSED) {
    adjust_other_scales(range, 6, gtk_range_get_value(range));
}

void on_colormap_scale7_value_changed(GtkRange *range,
                                      gpointer user_data HMG_ATTR_UNUSED) {
    adjust_other_scales(range, 7, gtk_range_get_value(range));
}

void on_colormap_scale8_value_changed(GtkRange *range,
                                      gpointer user_data HMG_ATTR_UNUSED) {
    adjust_other_scales(range, 8, gtk_range_get_value(range));
}

void on_colormap_update_button_clicked(GtkButton *button,
                                       gpointer user_data HMG_ATTR_UNUSED) {
    hmg_colormap_settings_t settings;
    GtkWidget *w;
    GdkColor color;
    GdkRectangle rect;
    int i;
    char name[]  = "colormap_colorbuttonx";
    char name2[] = "colormap_scalex";
    char nums[]  = "0123456789";

    for (i=1; i<=8; i++) {
        name[20] = nums[i];
        w = lookup_widget(GTK_WIDGET(button), name);
        gtk_color_button_get_color(GTK_COLOR_BUTTON(w), &color);
        settings.colors[i-1][0] = color.red >> 8;
        settings.colors[i-1][1] = color.green >> 8;
        settings.colors[i-1][2] = color.blue >> 8;

        name2[14] = nums[i];
        w = lookup_widget(GTK_WIDGET(button), name2);
        settings.scales[i-1] = gtk_range_get_value(GTK_RANGE(w));
    }

    hmg_init_colormap(&settings);

    w = lookup_widget(GTK_WIDGET(button), "colormap_display");
    rect.x = rect.y = 0;
    rect.width = 514;
    rect.height = 32;
    gdk_window_invalidate_rect(w->window, &rect, TRUE);
}

static void *rerender_thread(void *args) {
    set_main_progressbar(args);
    hmg_progress_meter = gui_progress_meter;

    render_map(args);

    activate_main_notebook(args, 1);
    return NULL;
}

void on_colormap_redraw_2dview_button_clicked(GtkButton *button,
                                              gpointer user_data
                                              HMG_ATTR_UNUSED) {
    deactivate_main_notebook(button);
    g_thread_create(rerender_thread, button, FALSE, NULL);
}

void on_colormap_auto_update_checkbutton_toggled(GtkToggleButton *togglebutton,
                                                 gpointer user_data
                                                 HMG_ATTR_UNUSED) {
    colormap_auto_update = gtk_toggle_button_get_active(togglebutton);
    if (colormap_auto_update)
        on_colormap_update_button_clicked(GTK_BUTTON(togglebutton), NULL);
}

void on_colormap_colorbutton_color_set(GtkColorButton  *colorbutton,
                                               gpointer user_data
                                               HMG_ATTR_UNUSED) {
    if (colormap_auto_update)
        on_colormap_update_button_clicked(GTK_BUTTON(colorbutton), NULL);
}

static void *export_thread(void *args) {
    GtkWidget *w;
    unsigned int format, ret;
    char *filename;

    set_main_progressbar(args);
    hmg_progress_meter = gui_progress_meter;

    w = lookup_widget(GTK_WIDGET(args), "export_format_combobox");
    format = gtk_combo_box_get_active(GTK_COMBO_BOX(w));
    w = lookup_widget(GTK_WIDGET(args), "export_filechooserwidget");
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(w));

    switch (format) {
    case 0:
        ret = hmg_export_pgm(filename, map, map_width, map_height);
        break;
    case 1:
        ret = hmg_export_ppm(filename, map, map_width, map_height);
        break;
    case 2:
        ret = hmg_export_bmp(filename, map, map_width, map_height);
        break;
    default:
        assert(0);
        break;
    }

    // FIXME: add some sort of hmg_errno and hmg_strerror or similar
    if (!ret)
        hmg_progress_meter("Error! Could not export to file!", 42);

    activate_main_notebook(args, 0);
    return NULL;
}

void on_export_save_button_clicked(GtkButton *button,
                                   gpointer user_data HMG_ATTR_UNUSED) {
    deactivate_main_notebook(button);
    g_thread_create(export_thread, button, FALSE, NULL);
}

static void *clip2_thread(void *args) {
    unsigned int clipmin, clipmax;

    set_main_progressbar(args);
    hmg_progress_meter = gui_progress_meter;

    gdk_threads_enter();
        clipmin = get_spin_button_int  (args, "clip2_min_spinbutton");
        clipmax = get_spin_button_int  (args, "clip2_max_spinbutton");
    gdk_threads_leave();

    hmg_clip(map, clipmin, clipmax, map_width, map_height);
    render_map(args);

    activate_main_notebook(args, 1);
    return NULL;
}

void on_clip_button_clicked(GtkButton *button,
                                   gpointer user_data HMG_ATTR_UNUSED) {
    deactivate_main_notebook(button);
    g_thread_create(clip2_thread, button, FALSE, NULL);
}

static void *crop2_thread(void *args) {
    unsigned int cropleft, cropright, croptop, cropbottom;

    set_main_progressbar(args);
    hmg_progress_meter = gui_progress_meter;

    gdk_threads_enter();
        cropleft   = get_spin_button_int  (args, "crop2_left_spinbutton");
        cropright  = get_spin_button_int  (args, "crop2_right_spinbutton");
        croptop    = get_spin_button_int  (args, "crop2_top_spinbutton");
        cropbottom = get_spin_button_int  (args, "crop2_bottom_spinbutton");
    gdk_threads_leave();

    sanitize_crop_settings(&cropleft, &cropright, &croptop, &cropbottom);
    hmg_crop(&map, &map_width, &map_height, cropleft, cropright, croptop, cropbottom);
    render_map(args);

    activate_main_notebook(args, 1);
    return NULL;
}

void on_crop_button_clicked(GtkButton *button,
                                   gpointer user_data HMG_ATTR_UNUSED) {
    deactivate_main_notebook(button);
    g_thread_create(crop2_thread, button, FALSE, NULL);
}
