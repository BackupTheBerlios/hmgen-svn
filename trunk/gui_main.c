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

#include <gtk/gtk.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "gui_interface.h"
#include "gui_support.h"
#include "lib_hmgen.c"

static char *image_path = NULL; 
static int malloced = 0;
static const char * const about_img = "/share/hmgen/";

static void assemble_image_path(char *argv0) {
    int x, y;
    char *p;

    x = strlen(argv0) - 1;

    if (x<=0) {
        image_path = "nothing";
        return;
    }

    while (x>0 && argv0[x] != '/' && argv0[x] != '\\') x--;

    y = x - 1;
    while (y>0 && argv0[y] != '/' && argv0[y] != '\\') y--;
    if (y) y++;
    if (!strncmp(&argv0[y], "bin", 3)) x = y;

    p = malloc(x + strlen(about_img) + 1);
    if (!p) {
        fprintf(stderr, "out of memory\n");
        exit(-1);
    }

    memcpy(p, argv0, x);
    snprintf(p+x, strlen(about_img) + 1, about_img);
    image_path = p;
    malloced = 1;
}

int main(int argc, char *argv[]) {
    GtkWidget *hmgengui;
    char *show_stderr;

    /* stop gtk spamming stderr with warnings about gnome icons
     * this is a bug in gtk */
    show_stderr = getenv("HMGENGUI_SHOW_STDERR");
    if (!show_stderr) {
        close(2);
        open("/dev/null", O_WRONLY);
    }

    gtk_disable_setlocale();
    setlocale(LC_ALL, "C");

    assemble_image_path(argv[0]);

    hmg_init_colormap(NULL);

    g_thread_init(NULL);
    gdk_threads_init();
    gdk_threads_enter();

    gtk_init(&argc, &argv);

    add_pixmap_directory (image_path);

    hmgengui = create_hmgengui();
    gtk_widget_show(hmgengui);

    gtk_main ();
    gdk_threads_leave();

    if (malloced) free(image_path);

    return 0;
}
