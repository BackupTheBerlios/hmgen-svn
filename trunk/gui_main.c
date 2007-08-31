#include <gtk/gtk.h>
#include <locale.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "gui_interface.h"
#include "gui_support.h"
#include "lib_hmgen.c"

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

    hmg_init_colormap();

    g_thread_init(NULL);
    gdk_threads_init();
    gdk_threads_enter();

    gtk_init(&argc, &argv);

    add_pixmap_directory (".");

    hmgengui = create_hmgengui();
    gtk_widget_show(hmgengui);

    gtk_main ();
    gdk_threads_leave();

    return 0;
}
