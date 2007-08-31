#ifndef HMG_GUI_CALLBACKS_H
#define HMG_GUI_CALLBACKS_H

#include <gtk/gtk.h>

void gui_quit(GtkWidget *widget, gpointer user_data);

void on_algorithm_combobox_changed(GtkComboBox *combobox, gpointer user_data);
void on_size_combobox_changed     (GtkComboBox *combobox, gpointer user_data);

void on_generate_button_clicked   (GtkButton *button, gpointer user_data);
void on_pgm_save_as_button_clicked(GtkButton *button, gpointer user_data);
void on_ppm_save_as_button_clicked(GtkButton *button, gpointer user_data);
void on_new_create_button_clicked (GtkButton *button, gpointer user_data);
void on_norm2_button_clicked      (GtkButton *button, gpointer user_data);
void on_blur2_button_clicked      (GtkButton *button, gpointer user_data);
void on_invert2_button_clicked    (GtkButton *button, gpointer user_data);
void on_colormap_update_button_clicked(GtkButton *button, gpointer user_data);
void on_colormap_redraw_2dview_button_clicked(GtkButton *button,
                                              gpointer user_data);
void on_colormap_auto_update_checkbutton_toggled(GtkToggleButton *togglebutton,
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
