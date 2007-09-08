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

#ifndef HMG_GUI_SUPPORT_H
#define HMG_GUI_SUPPORT_H

#include <gtk/gtk.h>

GtkWidget* lookup_widget(GtkWidget *widget, const gchar *widget_name);
void add_pixmap_directory(const gchar *directory);

GtkWidget*  create_pixmap(GtkWidget *widget, const gchar *filename);
GdkPixbuf*  create_pixbuf(const gchar *filename);

#endif
