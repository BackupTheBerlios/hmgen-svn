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

#ifndef HMG_GUI_CALLBACKS_H
#define HMG_GUI_CALLBACKS_H

#include <gtk/gtk.h>

void gui_quit(GtkWidget *widget, gpointer user_data);

void on_algorithm_combobox_changed(GtkComboBox *combobox, gpointer user_data);

void on_generate_button_clicked   (GtkButton *button, gpointer user_data);
void on_new_create_button_clicked (GtkButton *button, gpointer user_data);
void on_norm2_button_clicked      (GtkButton *button, gpointer user_data);
void on_blur2_button_clicked      (GtkButton *button, gpointer user_data);
void on_invert2_button_clicked    (GtkButton *button, gpointer user_data);
void on_clip_button_clicked       (GtkButton *button, gpointer user_data);
void on_crop_button_clicked       (GtkButton *button, gpointer user_data);
void on_level2_button_clicked     (GtkButton *button, gpointer user_data);
void on_export_save_button_clicked(GtkButton *button, gpointer user_data);
void on_colormap_update_button_clicked(GtkButton *button, gpointer user_data);
void on_colormap_redraw_2dview_button_clicked(GtkButton *button,
                                              gpointer user_data);
void on_colormap_auto_update_checkbutton_toggled(GtkToggleButton *togglebutton,
                                                 gpointer user_data);
void on_colormap_colorbutton_color_set(GtkColorButton  *colorbutton,
                                       gpointer user_data);

gboolean on_view_drawingarea_expose_event (GtkWidget *widget,
                                    GdkEventExpose *event, gpointer user_data);
gboolean on_colormap_display_expose_event (GtkWidget *widget,
                                    GdkEventExpose *event, gpointer user_data);

void on_colormap_scale1_value_changed(GtkRange *range, gpointer user_data);
void on_colormap_scale2_value_changed(GtkRange *range, gpointer user_data);
void on_colormap_scale3_value_changed(GtkRange *range, gpointer user_data);
void on_colormap_scale4_value_changed(GtkRange *range, gpointer user_data);
void on_colormap_scale5_value_changed(GtkRange *range, gpointer user_data);
void on_colormap_scale6_value_changed(GtkRange *range, gpointer user_data);
void on_colormap_scale7_value_changed(GtkRange *range, gpointer user_data);
void on_colormap_scale8_value_changed(GtkRange *range, gpointer user_data);

#endif
