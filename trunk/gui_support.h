#ifndef HMG_GUI_SUPPORT_H
#define HMG_GUI_SUPPORT_H

#include <gtk/gtk.h>

GtkWidget* lookup_widget(GtkWidget *widget, const gchar *widget_name);
void add_pixmap_directory(const gchar *directory);

GtkWidget*  create_pixmap(GtkWidget *widget, const gchar *filename);
GdkPixbuf*  create_pixbuf(const gchar *filename);

#endif
