#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include "gui_callbacks.h"
#include "gui_interface.h"
#include "gui_support.h"
#include "version.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

GtkWidget * create_hmgengui(void) {
  GtkWidget *hmgengui, *vbox1, *main_notebook;
  GtkWidget *view_scrolledwindow, *viewport1, *view_drawingarea, *view_label;
  GtkWidget *vbox3, *frame1, *alignment1, *hbox1, *label9, *algorithm_combobox;
  GtkWidget *label10, *size_combobox, *generate_button, *label6, *frame2;
  GtkWidget *alignment2, *postprocessing_notebook, *table7, *label26, *label27;
  GtkWidget *norm_min_spinbutton, *norm_max_spinbutton;
  GtkWidget *norm_first_checkbutton, *norm_last_checkbutton;
  GtkWidget *normalize_label, *table5, *blur_checkbutton, *label28, *label29;
  GtkWidget *label30, *blur_sigma_spinbutton, *blur_yradius_spinbutton;
  GtkWidget *blur_xradius_spinbutton, *blur_label, *table6;
  GtkWidget *invert_checkbutton, *invert_label, *label7, *frame3;
  GtkWidget *alignment3, *algosettings_notebook, *table1, *label11, *label12;
  GtkWidget *label13, *label14, *label15, *ff_seed_spinbutton;
  GtkWidget *ff_init_spinbutton, *ff_nplates_spinbutton;
  GtkWidget *ff_upfactor_spinbutton, *ff_downfactor_spinbutton;
  GtkWidget *ff_label, *table3, *label16, *label17, *label18, *label19;
  GtkWidget *label20, *label21, *label22, *mpd_roughness_spinbutton;
  GtkWidget *mpd_displacement_spinbutton, *mpd_br_spinbutton;
  GtkWidget *mpd_bl_spinbutton, *mpd_tr_spinbutton, *mpd_tl_spinbutton;
  GtkWidget *mpd_seed_spinbutton, *mpd_label, *table4, *label23, *label24;
  GtkWidget *label25, *forge_seed_spinbutton, *forge_fracdim_spinbutton;
  GtkWidget *forge_powscale_spinbutton, *forge_label, *label8, *generate_label;
  GtkWidget *table8, *save_label, *vbox4, *image3;
  GtkWidget *about_message, *about_label, *main_progressbar;
  GtkWidget *hbox5, *view_sidepanel_notebook, *table9, *label39, *label40;
  GtkWidget *new_width_spinbutton, *new_height_spinbutton, *new_create_button;
  GtkWidget *label35, *table10, *label36, *table11, *label37, *table12;
  GtkWidget *label38, *label41, *new_level_spinbutton, *table13, *label42;
  GtkWidget *label43, *label44, *norm2_min_spinbutton, *norm2_max_spinbutton;
  GtkWidget *norm2_button, *label45, *label46, *label47, *vbox5, *hbox7;
  GtkWidget *blur2_xrad_spinbutton, *blur2_yrad_spinbutton, *colormap_display;
  GtkWidget *blur2_sigma_spinbutton, *blur2_button, *label48, *invert2_button;
  GtkWidget *label49, *table14, *alignment6, *colormap_scale6;
  GtkWidget *colormap_colorbutton1, *colormap_colorbutton2;
  GtkWidget *colormap_colorbutton3, *colormap_colorbutton4;
  GtkWidget *colormap_colorbutton5, *colormap_colorbutton6;
  GtkWidget *colormap_colorbutton7, *colormap_colorbutton8;
  GtkWidget *colormap_scale1, *colormap_scale2, *colormap_scale3;
  GtkWidget *colormap_scale4, *colormap_scale5, *colormap_scale7;
  GtkWidget *colormap_scale8, *table15, *colormap_auto_update_checkbutton;
  GtkWidget *colormap_update_button, *colormap_redraw_2dview_button;
  GtkWidget *hpaned1, *hbox8, *label50, *export_format_combobox;
  GtkWidget *export_save_button, *export_filechooserwidget, *label51;

  GtkObject *norm_min_spinbutton_adj, *norm_max_spinbutton_adj;
  GtkObject *blur_sigma_spinbutton_adj, *blur_yradius_spinbutton_adj;
  GtkObject *blur_xradius_spinbutton_adj, *ff_seed_spinbutton_adj;
  GtkObject *ff_init_spinbutton_adj, *ff_nplates_spinbutton_adj;
  GtkObject *ff_upfactor_spinbutton_adj, *ff_downfactor_spinbutton_adj;
  GtkObject *mpd_roughness_spinbutton_adj, *mpd_displacement_spinbutton_adj;
  GtkObject *mpd_br_spinbutton_adj, *mpd_bl_spinbutton_adj;
  GtkObject *mpd_tr_spinbutton_adj, *mpd_tl_spinbutton_adj;
  GtkObject *mpd_seed_spinbutton_adj, *forge_seed_spinbutton_adj;
  GtkObject *forge_fracdim_spinbutton_adj, *forge_powscale_spinbutton_adj;
  GtkObject *new_width_spinbutton_adj, *new_height_spinbutton_adj;
  GtkObject *new_level_spinbutton_adj, *norm2_min_spinbutton_adj;
  GtkObject *norm2_max_spinbutton_adj, *blur2_xrad_spinbutton_adj;
  GtkObject *blur2_yrad_spinbutton_adj, *blur2_sigma_spinbutton_adj;

  hmgengui = gtk_window_new (GTK_WINDOW_TOPLEVEL);
//  gtk_widget_set_size_request (hmgengui, 800, -1);
  gtk_window_set_title (GTK_WINDOW (hmgengui), "hmgengui");
  gtk_window_set_position (GTK_WINDOW (hmgengui), GTK_WIN_POS_CENTER);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (hmgengui), vbox1);
//  gtk_widget_set_sensitive (vbox1, FALSE);

  main_notebook = gtk_notebook_new ();
  gtk_widget_show (main_notebook);
  gtk_box_pack_start (GTK_BOX (vbox1), main_notebook, TRUE, TRUE, 0);

  hbox5 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox5);
  gtk_container_add (GTK_CONTAINER (main_notebook), hbox5);

  view_sidepanel_notebook = gtk_notebook_new ();
  gtk_widget_show (view_sidepanel_notebook);
  gtk_box_pack_start (GTK_BOX (hbox5), view_sidepanel_notebook, FALSE, TRUE, 0);
  gtk_notebook_set_show_border (GTK_NOTEBOOK (view_sidepanel_notebook), FALSE);
  gtk_notebook_set_tab_pos (GTK_NOTEBOOK (view_sidepanel_notebook), GTK_POS_LEFT);

  table9 = gtk_table_new (5, 3, FALSE);
  gtk_widget_show (table9);
  gtk_container_add (GTK_CONTAINER (view_sidepanel_notebook), table9);
  gtk_container_set_border_width (GTK_CONTAINER (table9), 8);
  gtk_table_set_row_spacings (GTK_TABLE (table9), 8);
  gtk_table_set_col_spacings (GTK_TABLE (table9), 8);

  label39 = gtk_label_new ("Width");
  gtk_widget_show (label39);
  gtk_table_attach (GTK_TABLE (table9), label39, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label39), 0, 0.5);

  label40 = gtk_label_new ("Height");
  gtk_widget_show (label40);
  gtk_table_attach (GTK_TABLE (table9), label40, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label40), 0, 0.5);

  new_width_spinbutton_adj = gtk_adjustment_new (513, 1, 16385, 1, 10, 10);
  new_width_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (new_width_spinbutton_adj), 1, 0);
  gtk_widget_show (new_width_spinbutton);
  gtk_table_attach (GTK_TABLE (table9), new_width_spinbutton, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  new_height_spinbutton_adj = gtk_adjustment_new (513, 1, 16385, 1, 10, 10);
  new_height_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (new_height_spinbutton_adj), 1, 0);
  gtk_widget_show (new_height_spinbutton);
  gtk_table_attach (GTK_TABLE (table9), new_height_spinbutton, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  new_create_button = gtk_button_new_with_mnemonic ("Create");
  gtk_widget_show (new_create_button);
  gtk_table_attach (GTK_TABLE (table9), new_create_button, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label41 = gtk_label_new ("Level");
  gtk_widget_show (label41);
  gtk_table_attach (GTK_TABLE (table9), label41, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label41), 0, 0.5);

  new_level_spinbutton_adj = gtk_adjustment_new (128, 0, 255, 1, 10, 10);
  new_level_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (new_level_spinbutton_adj), 1, 0);
  gtk_widget_show (new_level_spinbutton);
  gtk_table_attach (GTK_TABLE (table9), new_level_spinbutton, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label35 = gtk_label_new ("New");
  gtk_widget_show (label35);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (view_sidepanel_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (view_sidepanel_notebook), 0), label35);

  table10 = gtk_table_new (4, 3, FALSE);
  gtk_widget_show (table10);
  gtk_container_add (GTK_CONTAINER (view_sidepanel_notebook), table10);
  gtk_container_set_border_width (GTK_CONTAINER (table10), 8);
  gtk_table_set_row_spacings (GTK_TABLE (table10), 8);
  gtk_table_set_col_spacings (GTK_TABLE (table10), 8);

  label43 = gtk_label_new ("Minimum");
  gtk_widget_show (label43);
  gtk_table_attach (GTK_TABLE (table10), label43, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label43), 0, 0.5);

  label44 = gtk_label_new ("Maximum");
  gtk_widget_show (label44);
  gtk_table_attach (GTK_TABLE (table10), label44, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label44), 0, 0.5);

  norm2_min_spinbutton_adj = gtk_adjustment_new (0, 0, 255, 1, 10, 10);
  norm2_min_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (norm2_min_spinbutton_adj), 1, 0);
  gtk_widget_show (norm2_min_spinbutton);
  gtk_table_attach (GTK_TABLE (table10), norm2_min_spinbutton, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  norm2_max_spinbutton_adj = gtk_adjustment_new (255, 0, 255, 1, 10, 10);
  norm2_max_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (norm2_max_spinbutton_adj), 1, 0);
  gtk_widget_show (norm2_max_spinbutton);
  gtk_table_attach (GTK_TABLE (table10), norm2_max_spinbutton, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  norm2_button = gtk_button_new_with_mnemonic ("Normalize");
  gtk_widget_show (norm2_button);
  gtk_table_attach (GTK_TABLE (table10), norm2_button, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label36 = gtk_label_new ("Normalize");
  gtk_widget_show (label36);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (view_sidepanel_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (view_sidepanel_notebook), 1), label36);

  table11 = gtk_table_new (5, 3, FALSE);
  gtk_widget_show (table11);
  gtk_container_add (GTK_CONTAINER (view_sidepanel_notebook), table11);
  gtk_container_set_border_width (GTK_CONTAINER (table11), 8);
  gtk_table_set_row_spacings (GTK_TABLE (table11), 8);
  gtk_table_set_col_spacings (GTK_TABLE (table11), 8);

  label45 = gtk_label_new ("X Radius");
  gtk_widget_show (label45);
  gtk_table_attach (GTK_TABLE (table11), label45, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label45), 0, 0.5);

  label46 = gtk_label_new ("Y Radius");
  gtk_widget_show (label46);
  gtk_table_attach (GTK_TABLE (table11), label46, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label46), 0, 0.5);

  label47 = gtk_label_new ("Sigma");
  gtk_widget_show (label47);
  gtk_table_attach (GTK_TABLE (table11), label47, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label47), 0, 0.5);

  blur2_xrad_spinbutton_adj = gtk_adjustment_new (3, 1, 128, 1, 10, 10);
  blur2_xrad_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (blur2_xrad_spinbutton_adj), 1, 0);
  gtk_widget_show (blur2_xrad_spinbutton);
  gtk_table_attach (GTK_TABLE (table11), blur2_xrad_spinbutton, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  blur2_yrad_spinbutton_adj = gtk_adjustment_new (3, 1, 128, 1, 10, 10);
  blur2_yrad_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (blur2_yrad_spinbutton_adj), 1, 0);
  gtk_widget_show (blur2_yrad_spinbutton);
  gtk_table_attach (GTK_TABLE (table11), blur2_yrad_spinbutton, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  blur2_sigma_spinbutton_adj = gtk_adjustment_new (1, 0, 100, 1, 10, 10);
  blur2_sigma_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (blur2_sigma_spinbutton_adj), 1, 2);
  gtk_widget_show (blur2_sigma_spinbutton);
  gtk_table_attach (GTK_TABLE (table11), blur2_sigma_spinbutton, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  blur2_button = gtk_button_new_with_mnemonic ("Blur");
  gtk_widget_show (blur2_button);
  gtk_table_attach (GTK_TABLE (table11), blur2_button, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label37 = gtk_label_new ("Blur");
  gtk_widget_show (label37);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (view_sidepanel_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (view_sidepanel_notebook), 2), label37);

  table12 = gtk_table_new (3, 3, FALSE);
  gtk_widget_show (table12);
  gtk_container_add (GTK_CONTAINER (view_sidepanel_notebook), table12);
  gtk_container_set_border_width (GTK_CONTAINER (table12), 8);
  gtk_table_set_row_spacings (GTK_TABLE (table12), 8);
  gtk_table_set_col_spacings (GTK_TABLE (table12), 8);

  label48 = gtk_label_new ("          ");
  gtk_widget_show (label48);
  gtk_table_attach (GTK_TABLE (table12), label48, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label48), 0, 0.5);

  invert2_button = gtk_button_new_with_mnemonic ("Invert");
  gtk_widget_show (invert2_button);
  gtk_table_attach (GTK_TABLE (table12), invert2_button, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label38 = gtk_label_new ("Invert");
  gtk_widget_show (label38);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (view_sidepanel_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (view_sidepanel_notebook), 3), label38);

  view_scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (view_scrolledwindow);
  gtk_box_pack_start (GTK_BOX (hbox5), view_scrolledwindow, TRUE, TRUE, 0);

  viewport1 = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport1);
  gtk_container_add (GTK_CONTAINER (view_scrolledwindow), viewport1);

  view_drawingarea = gtk_drawing_area_new ();
  gtk_widget_show (view_drawingarea);
  gtk_container_add (GTK_CONTAINER (viewport1), view_drawingarea);

  view_label = gtk_label_new ("2D View");
  gtk_widget_show (view_label);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (main_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (main_notebook), 0), view_label);

  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox3);
  gtk_container_add (GTK_CONTAINER (main_notebook), vbox3);

  frame1 = gtk_frame_new (NULL);
  gtk_widget_show (frame1);
  gtk_box_pack_start (GTK_BOX (vbox3), frame1, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame1), 8);

  alignment1 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (frame1), alignment1);
  gtk_container_set_border_width (GTK_CONTAINER (alignment1), 8);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment1), 0, 0, 12, 0);

  hbox1 = gtk_hbox_new (FALSE, 8);
  gtk_widget_show (hbox1);
  gtk_container_add (GTK_CONTAINER (alignment1), hbox1);

  label9 = gtk_label_new ("Algorithm");
  gtk_widget_show (label9);
  gtk_box_pack_start (GTK_BOX (hbox1), label9, FALSE, FALSE, 0);

  algorithm_combobox = gtk_combo_box_new_text ();
  gtk_widget_show (algorithm_combobox);
  gtk_box_pack_start (GTK_BOX (hbox1), algorithm_combobox, FALSE, FALSE, 0);
  gtk_combo_box_append_text (GTK_COMBO_BOX (algorithm_combobox), "Fault Formation");
  gtk_combo_box_append_text (GTK_COMBO_BOX (algorithm_combobox), "Midpoint Displacement (plasma)");
  gtk_combo_box_append_text (GTK_COMBO_BOX (algorithm_combobox), "Forge (ppmforge/gforge/fft'd noise)");

  label10 = gtk_label_new ("Size");
  gtk_widget_show (label10);
  gtk_box_pack_start (GTK_BOX (hbox1), label10, FALSE, FALSE, 0);

  size_combobox = gtk_combo_box_new_text ();
  gtk_widget_show (size_combobox);
  gtk_box_pack_start (GTK_BOX (hbox1), size_combobox, FALSE, FALSE, 0);
  gtk_combo_box_append_text (GTK_COMBO_BOX (size_combobox), "3 x 3");
  gtk_combo_box_append_text (GTK_COMBO_BOX (size_combobox), "5 x 5");
  gtk_combo_box_append_text (GTK_COMBO_BOX (size_combobox), "9 x 9");
  gtk_combo_box_append_text (GTK_COMBO_BOX (size_combobox), "17 x 17");
  gtk_combo_box_append_text (GTK_COMBO_BOX (size_combobox), "33 x 33");
  gtk_combo_box_append_text (GTK_COMBO_BOX (size_combobox), "65 x 65");
  gtk_combo_box_append_text (GTK_COMBO_BOX (size_combobox), "129 x 129");
  gtk_combo_box_append_text (GTK_COMBO_BOX (size_combobox), "257 x 257");
  gtk_combo_box_append_text (GTK_COMBO_BOX (size_combobox), "513 x 513");
  gtk_combo_box_append_text (GTK_COMBO_BOX (size_combobox), "1025 x 1025");
  gtk_combo_box_append_text (GTK_COMBO_BOX (size_combobox), "2049 x 2049");
  gtk_combo_box_append_text (GTK_COMBO_BOX (size_combobox), "4097 x 4097");
  gtk_combo_box_append_text (GTK_COMBO_BOX (size_combobox), "8193 x 8193");
  gtk_combo_box_append_text (GTK_COMBO_BOX (size_combobox), "16385 x 16384");

  generate_button = gtk_button_new_with_mnemonic ("Generate!");
  gtk_widget_show (generate_button);
  gtk_box_pack_start (GTK_BOX (hbox1), generate_button, FALSE, FALSE, 0);
  gtk_widget_set_sensitive (generate_button, FALSE);

  label6 = gtk_label_new ("Main");
  gtk_widget_show (label6);
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label6);
  gtk_label_set_use_markup (GTK_LABEL (label6), TRUE);

  frame2 = gtk_frame_new (NULL);
  gtk_widget_show (frame2);
  gtk_box_pack_start (GTK_BOX (vbox3), frame2, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame2), 8);

  alignment2 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment2);
  gtk_container_add (GTK_CONTAINER (frame2), alignment2);
  gtk_container_set_border_width (GTK_CONTAINER (alignment2), 8);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment2), 0, 0, 12, 0);

  postprocessing_notebook = gtk_notebook_new ();
  gtk_widget_show (postprocessing_notebook);
  gtk_container_add (GTK_CONTAINER (alignment2), postprocessing_notebook);

  table7 = gtk_table_new (4, 4, FALSE);
  gtk_widget_show (table7);
  gtk_container_add (GTK_CONTAINER (postprocessing_notebook), table7);
  gtk_container_set_border_width (GTK_CONTAINER (table7), 8);
  gtk_table_set_row_spacings (GTK_TABLE (table7), 8);
  gtk_table_set_col_spacings (GTK_TABLE (table7), 8);

  label26 = gtk_label_new ("Minimum");
  gtk_widget_show (label26);
  gtk_table_attach (GTK_TABLE (table7), label26, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label26), 0, 0.5);

  label27 = gtk_label_new ("Maximum");
  gtk_widget_show (label27);
  gtk_table_attach (GTK_TABLE (table7), label27, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label27), 0, 0.5);

  norm_min_spinbutton_adj = gtk_adjustment_new (0, 0, 255, 1, 10, 10);
  norm_min_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (norm_min_spinbutton_adj), 1, 0);
  gtk_widget_show (norm_min_spinbutton);
  gtk_table_attach (GTK_TABLE (table7), norm_min_spinbutton, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  norm_max_spinbutton_adj = gtk_adjustment_new (255, 0, 255, 1, 10, 10);
  norm_max_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (norm_max_spinbutton_adj), 1, 0);
  gtk_widget_show (norm_max_spinbutton);
  gtk_table_attach (GTK_TABLE (table7), norm_max_spinbutton, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  norm_first_checkbutton = gtk_check_button_new_with_mnemonic ("First");
  gtk_widget_show (norm_first_checkbutton);
  gtk_table_attach (GTK_TABLE (table7), norm_first_checkbutton, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  norm_last_checkbutton = gtk_check_button_new_with_mnemonic ("Last");
  gtk_widget_show (norm_last_checkbutton);
  gtk_table_attach (GTK_TABLE (table7), norm_last_checkbutton, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  normalize_label = gtk_label_new ("Normalize");
  gtk_widget_show (normalize_label);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (postprocessing_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (postprocessing_notebook), 0), normalize_label);

  table5 = gtk_table_new (5, 3, FALSE);
  gtk_widget_show (table5);
  gtk_container_add (GTK_CONTAINER (postprocessing_notebook), table5);
  gtk_container_set_border_width (GTK_CONTAINER (table5), 8);
  gtk_table_set_row_spacings (GTK_TABLE (table5), 8);
  gtk_table_set_col_spacings (GTK_TABLE (table5), 8);

  blur_checkbutton = gtk_check_button_new_with_mnemonic ("Enable");
  gtk_widget_show (blur_checkbutton);
  gtk_table_attach (GTK_TABLE (table5), blur_checkbutton, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label28 = gtk_label_new ("X Radius");
  gtk_widget_show (label28);
  gtk_table_attach (GTK_TABLE (table5), label28, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label28), 0, 0.5);

  label29 = gtk_label_new ("Y Radius");
  gtk_widget_show (label29);
  gtk_table_attach (GTK_TABLE (table5), label29, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label29), 0, 0.5);

  label30 = gtk_label_new ("Sigma");
  gtk_widget_show (label30);
  gtk_table_attach (GTK_TABLE (table5), label30, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label30), 0, 0.5);

  blur_sigma_spinbutton_adj = gtk_adjustment_new (1, 0, 100, 1, 10, 10);
  blur_sigma_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (blur_sigma_spinbutton_adj), 1, 2);
  gtk_widget_show (blur_sigma_spinbutton);
  gtk_table_attach (GTK_TABLE (table5), blur_sigma_spinbutton, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  blur_yradius_spinbutton_adj = gtk_adjustment_new (3, 1, 128, 1, 10, 10);
  blur_yradius_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (blur_yradius_spinbutton_adj), 1, 0);
  gtk_widget_show (blur_yradius_spinbutton);
  gtk_table_attach (GTK_TABLE (table5), blur_yradius_spinbutton, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  blur_xradius_spinbutton_adj = gtk_adjustment_new (3, 1, 128, 1, 10, 10);
  blur_xradius_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (blur_xradius_spinbutton_adj), 1, 0);
  gtk_widget_show (blur_xradius_spinbutton);
  gtk_table_attach (GTK_TABLE (table5), blur_xradius_spinbutton, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  blur_label = gtk_label_new ("Blur");
  gtk_widget_show (blur_label);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (postprocessing_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (postprocessing_notebook), 1), blur_label);

  table6 = gtk_table_new (2, 2, FALSE);
  gtk_widget_show (table6);
  gtk_container_add (GTK_CONTAINER (postprocessing_notebook), table6);
  gtk_container_set_border_width (GTK_CONTAINER (table6), 8);
  gtk_table_set_row_spacings (GTK_TABLE (table6), 8);
  gtk_table_set_col_spacings (GTK_TABLE (table6), 8);

  invert_checkbutton = gtk_check_button_new_with_mnemonic ("Enable");
  gtk_widget_show (invert_checkbutton);
  gtk_table_attach (GTK_TABLE (table6), invert_checkbutton, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  invert_label = gtk_label_new ("Invert");
  gtk_widget_show (invert_label);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (postprocessing_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (postprocessing_notebook), 2), invert_label);

  label7 = gtk_label_new ("Postprocessing");
  gtk_widget_show (label7);
  gtk_frame_set_label_widget (GTK_FRAME (frame2), label7);
  gtk_label_set_use_markup (GTK_LABEL (label7), TRUE);

  frame3 = gtk_frame_new (NULL);
  gtk_widget_show (frame3);
  gtk_box_pack_start (GTK_BOX (vbox3), frame3, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame3), 8);

  alignment3 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment3);
  gtk_container_add (GTK_CONTAINER (frame3), alignment3);
  gtk_container_set_border_width (GTK_CONTAINER (alignment3), 8);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment3), 0, 0, 12, 0);

  algosettings_notebook = gtk_notebook_new ();
  gtk_notebook_set_show_tabs(GTK_NOTEBOOK(algosettings_notebook), FALSE);
  gtk_widget_show (algosettings_notebook);
  gtk_container_add (GTK_CONTAINER (alignment3), algosettings_notebook);

  table1 = gtk_table_new (6, 3, FALSE);
  gtk_widget_show (table1);
  gtk_container_add (GTK_CONTAINER (algosettings_notebook), table1);
  gtk_container_set_border_width (GTK_CONTAINER (table1), 8);
  gtk_table_set_row_spacings (GTK_TABLE (table1), 8);
  gtk_table_set_col_spacings (GTK_TABLE (table1), 8);

  label11 = gtk_label_new ("Seed");
  gtk_widget_show (label11);
  gtk_table_attach (GTK_TABLE (table1), label11, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label11), 0, 0.5);

  label12 = gtk_label_new ("Initial Height");
  gtk_widget_show (label12);
  gtk_table_attach (GTK_TABLE (table1), label12, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label12), 0, 0.5);

  label13 = gtk_label_new ("Number of Plates");
  gtk_widget_show (label13);
  gtk_table_attach (GTK_TABLE (table1), label13, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label13), 0, 0.5);

  label14 = gtk_label_new ("Up Factor");
  gtk_widget_show (label14);
  gtk_table_attach (GTK_TABLE (table1), label14, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label14), 0, 0.5);

  label15 = gtk_label_new ("Down Factor");
  gtk_widget_show (label15);
  gtk_table_attach (GTK_TABLE (table1), label15, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label15), 0, 0.5);

  ff_seed_spinbutton_adj = gtk_adjustment_new (42, 0, 2147469952, 1, 10, 10);
  ff_seed_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (ff_seed_spinbutton_adj), 1, 0);
  gtk_widget_show (ff_seed_spinbutton);
  gtk_table_attach (GTK_TABLE (table1), ff_seed_spinbutton, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  ff_init_spinbutton_adj = gtk_adjustment_new (128, 0, 255, 1, 10, 10);
  ff_init_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (ff_init_spinbutton_adj), 1, 0);
  gtk_widget_show (ff_init_spinbutton);
  gtk_table_attach (GTK_TABLE (table1), ff_init_spinbutton, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  ff_nplates_spinbutton_adj = gtk_adjustment_new (512, 1, 65536, 1, 10, 10);
  ff_nplates_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (ff_nplates_spinbutton_adj), 1, 0);
  gtk_widget_show (ff_nplates_spinbutton);
  gtk_table_attach (GTK_TABLE (table1), ff_nplates_spinbutton, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  ff_upfactor_spinbutton_adj = gtk_adjustment_new (1, 0, 255, 1, 10, 10);
  ff_upfactor_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (ff_upfactor_spinbutton_adj), 1, 0);
  gtk_widget_show (ff_upfactor_spinbutton);
  gtk_table_attach (GTK_TABLE (table1), ff_upfactor_spinbutton, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  ff_downfactor_spinbutton_adj = gtk_adjustment_new (1, 0, 255, 1, 10, 10);
  ff_downfactor_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (ff_downfactor_spinbutton_adj), 1, 0);
  gtk_widget_show (ff_downfactor_spinbutton);
  gtk_table_attach (GTK_TABLE (table1), ff_downfactor_spinbutton, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  ff_label = gtk_label_new ("FF");
  gtk_widget_show (ff_label);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (algosettings_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (algosettings_notebook), 0), ff_label);

  table3 = gtk_table_new (8, 3, FALSE);
  gtk_widget_show (table3);
  gtk_container_add (GTK_CONTAINER (algosettings_notebook), table3);
  gtk_container_set_border_width (GTK_CONTAINER (table3), 8);
  gtk_table_set_row_spacings (GTK_TABLE (table3), 8);
  gtk_table_set_col_spacings (GTK_TABLE (table3), 8);

  label16 = gtk_label_new ("Seed");
  gtk_widget_show (label16);
  gtk_table_attach (GTK_TABLE (table3), label16, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label16), 0, 0.5);

  label17 = gtk_label_new ("Top Left");
  gtk_widget_show (label17);
  gtk_table_attach (GTK_TABLE (table3), label17, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label17), 0, 0.5);

  label18 = gtk_label_new ("Top Right");
  gtk_widget_show (label18);
  gtk_table_attach (GTK_TABLE (table3), label18, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label18), 0, 0.5);

  label19 = gtk_label_new ("Bottom Left");
  gtk_widget_show (label19);
  gtk_table_attach (GTK_TABLE (table3), label19, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label19), 0, 0.5);

  label20 = gtk_label_new ("Bottom Right");
  gtk_widget_show (label20);
  gtk_table_attach (GTK_TABLE (table3), label20, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label20), 0, 0.5);

  label21 = gtk_label_new ("Displacement");
  gtk_widget_show (label21);
  gtk_table_attach (GTK_TABLE (table3), label21, 0, 1, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label21), 0, 0.5);

  label22 = gtk_label_new ("Roughness");
  gtk_widget_show (label22);
  gtk_table_attach (GTK_TABLE (table3), label22, 0, 1, 6, 7,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label22), 0, 0.5);

  mpd_roughness_spinbutton_adj = gtk_adjustment_new (0.6, 0, 2, 0.00999999977648, 0.10000000149, 1);
  mpd_roughness_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (mpd_roughness_spinbutton_adj), 1, 2);
  gtk_widget_show (mpd_roughness_spinbutton);
  gtk_table_attach (GTK_TABLE (table3), mpd_roughness_spinbutton, 1, 2, 6, 7,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  mpd_displacement_spinbutton_adj = gtk_adjustment_new (64, 0, 255, 1, 10, 10);
  mpd_displacement_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (mpd_displacement_spinbutton_adj), 1, 0);
  gtk_widget_show (mpd_displacement_spinbutton);
  gtk_table_attach (GTK_TABLE (table3), mpd_displacement_spinbutton, 1, 2, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  mpd_br_spinbutton_adj = gtk_adjustment_new (128, 0, 255, 1, 10, 10);
  mpd_br_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (mpd_br_spinbutton_adj), 1, 0);
  gtk_widget_show (mpd_br_spinbutton);
  gtk_table_attach (GTK_TABLE (table3), mpd_br_spinbutton, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  mpd_bl_spinbutton_adj = gtk_adjustment_new (128, 0, 255, 1, 10, 10);
  mpd_bl_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (mpd_bl_spinbutton_adj), 1, 0);
  gtk_widget_show (mpd_bl_spinbutton);
  gtk_table_attach (GTK_TABLE (table3), mpd_bl_spinbutton, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  mpd_tr_spinbutton_adj = gtk_adjustment_new (128, 0, 255, 1, 10, 10);
  mpd_tr_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (mpd_tr_spinbutton_adj), 1, 0);
  gtk_widget_show (mpd_tr_spinbutton);
  gtk_table_attach (GTK_TABLE (table3), mpd_tr_spinbutton, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  mpd_tl_spinbutton_adj = gtk_adjustment_new (128, 0, 255, 1, 10, 10);
  mpd_tl_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (mpd_tl_spinbutton_adj), 1, 0);
  gtk_widget_show (mpd_tl_spinbutton);
  gtk_table_attach (GTK_TABLE (table3), mpd_tl_spinbutton, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  mpd_seed_spinbutton_adj = gtk_adjustment_new (42, 0, 2147469952, 1, 10, 10);
  mpd_seed_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (mpd_seed_spinbutton_adj), 1, 0);
  gtk_widget_show (mpd_seed_spinbutton);
  gtk_table_attach (GTK_TABLE (table3), mpd_seed_spinbutton, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  mpd_label = gtk_label_new ("MPD");
  gtk_widget_show (mpd_label);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (algosettings_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (algosettings_notebook), 1), mpd_label);

  table4 = gtk_table_new (4, 3, FALSE);
  gtk_widget_show (table4);
  gtk_container_add (GTK_CONTAINER (algosettings_notebook), table4);
  gtk_container_set_border_width (GTK_CONTAINER (table4), 8);
  gtk_table_set_row_spacings (GTK_TABLE (table4), 8);
  gtk_table_set_col_spacings (GTK_TABLE (table4), 8);

  label23 = gtk_label_new ("Seed");
  gtk_widget_show (label23);
  gtk_table_attach (GTK_TABLE (table4), label23, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label23), 0, 0.5);

  label24 = gtk_label_new ("Fractal Dimension");
  gtk_widget_show (label24);
  gtk_table_attach (GTK_TABLE (table4), label24, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label24), 0, 0.5);

  label25 = gtk_label_new ("Power Scale");
  gtk_widget_show (label25);
  gtk_table_attach (GTK_TABLE (table4), label25, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label25), 0, 0.5);

  forge_seed_spinbutton_adj = gtk_adjustment_new (47, 0, 2147469952, 1, 10, 10);
  forge_seed_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (forge_seed_spinbutton_adj), 1, 0);
  gtk_widget_show (forge_seed_spinbutton);
  gtk_table_attach (GTK_TABLE (table4), forge_seed_spinbutton, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  forge_fracdim_spinbutton_adj = gtk_adjustment_new (2.40000009537, 0, 3, 0.00999999977648, 0.10000000149, 1);
  forge_fracdim_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (forge_fracdim_spinbutton_adj), 1, 2);
  gtk_widget_show (forge_fracdim_spinbutton);
  gtk_table_attach (GTK_TABLE (table4), forge_fracdim_spinbutton, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  forge_powscale_spinbutton_adj = gtk_adjustment_new (1, 0, 8, 0.00999999977648, 0.10000000149, 1);
  forge_powscale_spinbutton = gtk_spin_button_new (GTK_ADJUSTMENT (forge_powscale_spinbutton_adj), 1, 2);
  gtk_widget_show (forge_powscale_spinbutton);
  gtk_table_attach (GTK_TABLE (table4), forge_powscale_spinbutton, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  forge_label = gtk_label_new ("Forge");
  gtk_widget_show (forge_label);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (algosettings_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (algosettings_notebook), 2), forge_label);

  label8 = gtk_label_new ("Algorithm Settings");
  gtk_widget_show (label8);
  gtk_frame_set_label_widget (GTK_FRAME (frame3), label8);
  gtk_label_set_use_markup (GTK_LABEL (label8), TRUE);

  generate_label = gtk_label_new ("Generate");
  gtk_widget_show (generate_label);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (main_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (main_notebook), 1), generate_label);

  vbox5 = gtk_vbox_new (FALSE, 8);
  gtk_widget_show (vbox5);
  gtk_container_add (GTK_CONTAINER (main_notebook), vbox5);
  gtk_container_set_border_width (GTK_CONTAINER (vbox5), 8);

  hbox7 = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox7);
  gtk_box_pack_start (GTK_BOX (vbox5), hbox7, FALSE, FALSE, 0);

  colormap_display = gtk_drawing_area_new();
  gtk_widget_show (colormap_display);
  gtk_box_pack_start (GTK_BOX (hbox7), colormap_display, FALSE, FALSE, 0);
  gtk_widget_set_size_request (colormap_display, 258, 32);

  table14 = gtk_table_new (9, 3, FALSE);
  gtk_widget_show (table14);
  gtk_box_pack_start (GTK_BOX (vbox5), table14, FALSE, TRUE, 0);
  gtk_table_set_row_spacings (GTK_TABLE (table14), 8);
  gtk_table_set_col_spacings (GTK_TABLE (table14), 8);

  alignment6 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment6);
  gtk_table_attach (GTK_TABLE (table14), alignment6, 1, 2, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  colormap_scale6 = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 255, 0, 0, 0)));
  gtk_widget_show (colormap_scale6);
  gtk_container_add (GTK_CONTAINER (alignment6), colormap_scale6);
  gtk_scale_set_digits (GTK_SCALE (colormap_scale6), 0);

  colormap_colorbutton1 = gtk_color_button_new ();
  gtk_widget_show (colormap_colorbutton1);
  gtk_table_attach (GTK_TABLE (table14), colormap_colorbutton1, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  
  colormap_colorbutton2 = gtk_color_button_new ();
  gtk_widget_show (colormap_colorbutton2);
  gtk_table_attach (GTK_TABLE (table14), colormap_colorbutton2, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  colormap_colorbutton3 = gtk_color_button_new ();
  gtk_widget_show (colormap_colorbutton3);
  gtk_table_attach (GTK_TABLE (table14), colormap_colorbutton3, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  colormap_colorbutton4 = gtk_color_button_new ();
  gtk_widget_show (colormap_colorbutton4);
  gtk_table_attach (GTK_TABLE (table14), colormap_colorbutton4, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  colormap_colorbutton5 = gtk_color_button_new ();
  gtk_widget_show (colormap_colorbutton5);
  gtk_table_attach (GTK_TABLE (table14), colormap_colorbutton5, 0, 1, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  colormap_colorbutton6 = gtk_color_button_new ();
  gtk_widget_show (colormap_colorbutton6);
  gtk_table_attach (GTK_TABLE (table14), colormap_colorbutton6, 0, 1, 5, 6,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  colormap_colorbutton7 = gtk_color_button_new ();
  gtk_widget_show (colormap_colorbutton7);
  gtk_table_attach (GTK_TABLE (table14), colormap_colorbutton7, 0, 1, 6, 7,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  colormap_colorbutton8 = gtk_color_button_new ();
  gtk_widget_show (colormap_colorbutton8);
  gtk_table_attach (GTK_TABLE (table14), colormap_colorbutton8, 0, 1, 7, 8,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  colormap_scale2 = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 255, 0, 0, 0)));
  gtk_widget_show (colormap_scale2);
  gtk_table_attach (GTK_TABLE (table14), colormap_scale2, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_scale_set_digits (GTK_SCALE (colormap_scale2), 0);

  colormap_scale3 = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 255, 0, 0, 0)));
  gtk_widget_show (colormap_scale3);
  gtk_table_attach (GTK_TABLE (table14), colormap_scale3, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_scale_set_digits (GTK_SCALE (colormap_scale3), 0);

  colormap_scale4 = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 255, 0, 0, 0)));
  gtk_widget_show (colormap_scale4);
  gtk_table_attach (GTK_TABLE (table14), colormap_scale4, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_scale_set_digits (GTK_SCALE (colormap_scale4), 0);

  colormap_scale5 = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 255, 0, 0, 0)));
  gtk_widget_show (colormap_scale5);
  gtk_table_attach (GTK_TABLE (table14), colormap_scale5, 1, 2, 4, 5,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_scale_set_digits (GTK_SCALE (colormap_scale5), 0);

  colormap_scale7  = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 255, 0, 0, 0)));
  gtk_widget_show (colormap_scale7);
  gtk_table_attach (GTK_TABLE (table14), colormap_scale7, 1, 2, 6, 7,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_scale_set_digits (GTK_SCALE (colormap_scale7), 0);

  colormap_scale8 = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 255, 0, 0, 0)));
  gtk_widget_show (colormap_scale8);
  gtk_table_attach (GTK_TABLE (table14), colormap_scale8, 1, 2, 7, 8,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_scale_set_digits (GTK_SCALE (colormap_scale8), 0);

  colormap_scale1 = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (0, 0, 255, 0, 0, 0)));
  gtk_widget_show (colormap_scale1);
  gtk_table_attach (GTK_TABLE (table14), colormap_scale1, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_widget_set_size_request (colormap_scale1, 300, -1);
  gtk_scale_set_digits (GTK_SCALE (colormap_scale1), 0);

  table15 = gtk_table_new (2, 4, FALSE);
  gtk_widget_show (table15);
  gtk_box_pack_start (GTK_BOX (vbox5), table15, TRUE, TRUE, 0);
  gtk_table_set_row_spacings (GTK_TABLE (table15), 8);
  gtk_table_set_col_spacings (GTK_TABLE (table15), 8);

  colormap_auto_update_checkbutton = gtk_check_button_new_with_mnemonic ("Auto Update");
  gtk_widget_show (colormap_auto_update_checkbutton);
  gtk_table_attach (GTK_TABLE (table15), colormap_auto_update_checkbutton, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  colormap_update_button = gtk_button_new_with_mnemonic ("Update");
  gtk_widget_show (colormap_update_button);
  gtk_table_attach (GTK_TABLE (table15), colormap_update_button, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  colormap_redraw_2dview_button = gtk_button_new_with_mnemonic ("Redraw 2D View");
  gtk_widget_show (colormap_redraw_2dview_button);
  gtk_table_attach (GTK_TABLE (table15), colormap_redraw_2dview_button, 2, 3, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label49 = gtk_label_new ("Colormap");
  gtk_widget_show (label49);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (main_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (main_notebook), 2), label49);

  table13 = gtk_table_new (3, 3, FALSE);
//  gtk_widget_show (table13);
  gtk_container_add (GTK_CONTAINER (main_notebook), table13);

  label42 = gtk_label_new ("Import");
  gtk_widget_show (label42);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (main_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (main_notebook), 3), label42);

  hpaned1 = gtk_hpaned_new ();
  gtk_widget_show (hpaned1);
  gtk_container_add (GTK_CONTAINER (main_notebook), hpaned1);
  gtk_paned_set_position (GTK_PANED (hpaned1), 1000);

  table8 = gtk_table_new (3, 2, FALSE);
  gtk_widget_show (table8);
  gtk_paned_pack1 (GTK_PANED (hpaned1), table8, FALSE, TRUE);
  gtk_container_set_border_width (GTK_CONTAINER (table8), 8);
  gtk_table_set_row_spacings (GTK_TABLE (table8), 8);
  gtk_table_set_col_spacings (GTK_TABLE (table8), 8);

  hbox8 = gtk_hbox_new (FALSE, 8);
  gtk_widget_show (hbox8);
  gtk_table_attach (GTK_TABLE (table8), hbox8, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  label50 = gtk_label_new ("");
  gtk_widget_show (label50);
  gtk_box_pack_start (GTK_BOX (hbox8), label50, TRUE, TRUE, 0);

  export_format_combobox = gtk_combo_box_new_text ();
  gtk_widget_show (export_format_combobox);
  gtk_box_pack_start (GTK_BOX (hbox8), export_format_combobox, FALSE, TRUE, 0);
  gtk_combo_box_append_text (GTK_COMBO_BOX (export_format_combobox), "PGM");
  gtk_combo_box_append_text (GTK_COMBO_BOX (export_format_combobox), "PPM");
  gtk_combo_box_set_active(GTK_COMBO_BOX(export_format_combobox), 0);

  export_save_button = gtk_button_new_from_stock ("gtk-save");
  gtk_widget_show (export_save_button);
  gtk_box_pack_start (GTK_BOX (hbox8), export_save_button, FALSE, FALSE, 0);
  gtk_widget_set_sensitive (export_save_button, FALSE);

  export_filechooserwidget = gtk_file_chooser_widget_new (GTK_FILE_CHOOSER_ACTION_SAVE);
  gtk_widget_show (export_filechooserwidget);
  gtk_table_attach (GTK_TABLE (table8), export_filechooserwidget, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);

  label51 = gtk_label_new ("");
  gtk_widget_show (label51);
  gtk_paned_pack2 (GTK_PANED (hpaned1), label51, TRUE, TRUE);

  save_label = gtk_label_new ("Export");
  gtk_widget_show (save_label);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (main_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (main_notebook), 4), save_label);

  vbox4 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox4);
  gtk_container_add (GTK_CONTAINER (main_notebook), vbox4);

  image3 = create_pixmap (hmgengui, "hmgen.png");
  gtk_widget_show (image3);
  gtk_box_pack_start (GTK_BOX (vbox4), image3, FALSE, FALSE, 64);

  about_message = gtk_label_new(HMG_VERSION_STRING);
  gtk_widget_show (about_message);
  gtk_box_pack_start (GTK_BOX (vbox4), about_message, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (about_message), GTK_JUSTIFY_CENTER);

  about_label = gtk_label_new ("About...");
  gtk_widget_show (about_label);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (main_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (main_notebook), 5), about_label);

  main_progressbar = gtk_progress_bar_new ();
  gtk_widget_show (main_progressbar);
  gtk_box_pack_start (GTK_BOX (vbox1), main_progressbar, FALSE, FALSE, 0);
  gtk_progress_bar_set_text (GTK_PROGRESS_BAR (main_progressbar), "Idle...");

  g_signal_connect ((gpointer) hmgengui, "delete_event",
                    G_CALLBACK (gui_quit),
                    NULL);
  g_signal_connect ((gpointer) view_drawingarea, "expose_event",
                    G_CALLBACK (on_view_drawingarea_expose_event),
                    NULL);
  g_signal_connect ((gpointer) algorithm_combobox, "changed",
                    G_CALLBACK (on_algorithm_combobox_changed),
                    NULL);
  g_signal_connect ((gpointer) size_combobox, "changed",
                    G_CALLBACK (on_size_combobox_changed),
                    NULL);
  g_signal_connect ((gpointer) generate_button, "clicked",
                    G_CALLBACK (on_generate_button_clicked),
                    NULL);
  g_signal_connect ((gpointer) new_create_button, "clicked",
                    G_CALLBACK (on_new_create_button_clicked),
                    NULL);
  g_signal_connect ((gpointer) norm2_button, "clicked",
                    G_CALLBACK (on_norm2_button_clicked),
                    NULL);
  g_signal_connect ((gpointer) blur2_button, "clicked",
                    G_CALLBACK (on_blur2_button_clicked),
                    NULL);
  g_signal_connect ((gpointer) invert2_button, "clicked",
                    G_CALLBACK (on_invert2_button_clicked),
                    NULL);
  g_signal_connect ((gpointer) colormap_display, "expose_event",
                    G_CALLBACK (on_colormap_display_expose_event),
                    NULL);
  g_signal_connect ((gpointer) colormap_scale1, "value_changed",
                    G_CALLBACK (on_colormap_scale1_value_changed),
                    NULL);
  g_signal_connect ((gpointer) colormap_scale2, "value_changed",
                    G_CALLBACK (on_colormap_scale2_value_changed),
                    NULL);
  g_signal_connect ((gpointer) colormap_scale3, "value_changed",
                    G_CALLBACK (on_colormap_scale3_value_changed),
                    NULL);
  g_signal_connect ((gpointer) colormap_scale4, "value_changed",
                    G_CALLBACK (on_colormap_scale4_value_changed),
                    NULL);
  g_signal_connect ((gpointer) colormap_scale5, "value_changed",
                    G_CALLBACK (on_colormap_scale5_value_changed),
                    NULL);
  g_signal_connect ((gpointer) colormap_scale6, "value_changed",
                    G_CALLBACK (on_colormap_scale6_value_changed),
                    NULL);
  g_signal_connect ((gpointer) colormap_scale7, "value_changed",
                    G_CALLBACK (on_colormap_scale7_value_changed),
                    NULL);
  g_signal_connect ((gpointer) colormap_scale8, "value_changed",
                    G_CALLBACK (on_colormap_scale8_value_changed),
                    NULL);
  g_signal_connect ((gpointer) colormap_update_button, "clicked",
                    G_CALLBACK (on_colormap_update_button_clicked),
                    NULL);
  g_signal_connect ((gpointer) colormap_redraw_2dview_button, "clicked",
                    G_CALLBACK (on_colormap_redraw_2dview_button_clicked),
                    NULL);
  g_signal_connect ((gpointer) colormap_auto_update_checkbutton, "toggled",
                    G_CALLBACK (on_colormap_auto_update_checkbutton_toggled),
                    NULL);
  g_signal_connect ((gpointer) colormap_colorbutton1, "color_set",
                    G_CALLBACK (on_colormap_colorbutton_color_set), NULL);
  g_signal_connect ((gpointer) colormap_colorbutton2, "color_set",
                    G_CALLBACK (on_colormap_colorbutton_color_set), NULL);
  g_signal_connect ((gpointer) colormap_colorbutton3, "color_set",
                    G_CALLBACK (on_colormap_colorbutton_color_set), NULL);
  g_signal_connect ((gpointer) colormap_colorbutton4, "color_set",
                    G_CALLBACK (on_colormap_colorbutton_color_set), NULL);
  g_signal_connect ((gpointer) colormap_colorbutton5, "color_set",
                    G_CALLBACK (on_colormap_colorbutton_color_set), NULL);
  g_signal_connect ((gpointer) colormap_colorbutton6, "color_set",
                    G_CALLBACK (on_colormap_colorbutton_color_set), NULL);
  g_signal_connect ((gpointer) colormap_colorbutton7, "color_set",
                    G_CALLBACK (on_colormap_colorbutton_color_set), NULL);
  g_signal_connect ((gpointer) colormap_colorbutton8, "color_set",
                    G_CALLBACK (on_colormap_colorbutton_color_set), NULL);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (hmgengui, hmgengui, "hmgengui");
  GLADE_HOOKUP_OBJECT (hmgengui, main_notebook, "main_notebook");
  GLADE_HOOKUP_OBJECT (hmgengui, view_scrolledwindow, "view_scrolledwindow");
  GLADE_HOOKUP_OBJECT (hmgengui, view_drawingarea, "view_drawingarea");
  GLADE_HOOKUP_OBJECT (hmgengui, view_label, "view_label");
  GLADE_HOOKUP_OBJECT (hmgengui, algorithm_combobox, "algorithm_combobox");
  GLADE_HOOKUP_OBJECT (hmgengui, size_combobox, "size_combobox");
  GLADE_HOOKUP_OBJECT (hmgengui, generate_button, "generate_button");
  GLADE_HOOKUP_OBJECT (hmgengui, postprocessing_notebook, "postprocessing_notebook");
  GLADE_HOOKUP_OBJECT (hmgengui, norm_min_spinbutton, "norm_min_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, norm_max_spinbutton, "norm_max_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, norm_first_checkbutton, "norm_first_checkbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, norm_last_checkbutton, "norm_last_checkbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, normalize_label, "normalize_label");
  GLADE_HOOKUP_OBJECT (hmgengui, blur_checkbutton, "blur_checkbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, blur_sigma_spinbutton, "blur_sigma_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, blur_yradius_spinbutton, "blur_yradius_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, blur_xradius_spinbutton, "blur_xradius_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, blur_label, "blur_label");
  GLADE_HOOKUP_OBJECT (hmgengui, invert_checkbutton, "invert_checkbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, invert_label, "invert_label");
  GLADE_HOOKUP_OBJECT (hmgengui, algosettings_notebook, "algosettings_notebook");
  GLADE_HOOKUP_OBJECT (hmgengui, ff_seed_spinbutton, "ff_seed_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, ff_init_spinbutton, "ff_init_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, ff_nplates_spinbutton, "ff_nplates_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, ff_upfactor_spinbutton, "ff_upfactor_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, ff_downfactor_spinbutton, "ff_downfactor_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, ff_label, "ff_label");
  GLADE_HOOKUP_OBJECT (hmgengui, mpd_roughness_spinbutton, "mpd_roughness_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, mpd_displacement_spinbutton, "mpd_displacement_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, mpd_br_spinbutton, "mpd_br_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, mpd_bl_spinbutton, "mpd_bl_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, mpd_tr_spinbutton, "mpd_tr_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, mpd_tl_spinbutton, "mpd_tl_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, mpd_seed_spinbutton, "mpd_seed_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, mpd_label, "mpd_label");
  GLADE_HOOKUP_OBJECT (hmgengui, forge_seed_spinbutton, "forge_seed_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, forge_fracdim_spinbutton, "forge_fracdim_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, forge_powscale_spinbutton, "forge_powscale_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, forge_label, "forge_label");
  GLADE_HOOKUP_OBJECT (hmgengui, generate_label, "generate_label");
  GLADE_HOOKUP_OBJECT (hmgengui, save_label, "save_label");
  GLADE_HOOKUP_OBJECT (hmgengui, about_message, "about_message");
  GLADE_HOOKUP_OBJECT (hmgengui, about_label, "about_label");
  GLADE_HOOKUP_OBJECT (hmgengui, main_progressbar, "main_progressbar");
  GLADE_HOOKUP_OBJECT (hmgengui, view_sidepanel_notebook, "view_sidepanel_note book");
  GLADE_HOOKUP_OBJECT (hmgengui, new_width_spinbutton, "new_width_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, new_height_spinbutton, "new_height_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, new_create_button, "new_create_button");
  GLADE_HOOKUP_OBJECT (hmgengui, new_level_spinbutton, "new_level_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, norm2_min_spinbutton, "norm2_min_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, norm2_max_spinbutton, "norm2_max_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, norm2_button, "norm2_button");
  GLADE_HOOKUP_OBJECT (hmgengui, blur2_xrad_spinbutton, "blur2_xrad_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, blur2_yrad_spinbutton, "blur2_yrad_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, blur2_sigma_spinbutton, "blur2_sigma_spinbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, blur2_button, "blur2_button");
  GLADE_HOOKUP_OBJECT (hmgengui, invert2_button, "invert2_button");
  GLADE_HOOKUP_OBJECT (hmgengui, colormap_display, "colormap_display");

  GLADE_HOOKUP_OBJECT (hmgengui, colormap_colorbutton1, "colormap_colorbutton1");
  GLADE_HOOKUP_OBJECT (hmgengui, colormap_colorbutton2, "colormap_colorbutton2");
  GLADE_HOOKUP_OBJECT (hmgengui, colormap_colorbutton3, "colormap_colorbutton3");
  GLADE_HOOKUP_OBJECT (hmgengui, colormap_colorbutton4, "colormap_colorbutton4");
  GLADE_HOOKUP_OBJECT (hmgengui, colormap_colorbutton5, "colormap_colorbutton5");
  GLADE_HOOKUP_OBJECT (hmgengui, colormap_colorbutton6, "colormap_colorbutton6");
  GLADE_HOOKUP_OBJECT (hmgengui, colormap_colorbutton7, "colormap_colorbutton7");
  GLADE_HOOKUP_OBJECT (hmgengui, colormap_colorbutton8, "colormap_colorbutton8");
  GLADE_HOOKUP_OBJECT (hmgengui, colormap_scale1, "colormap_scale1");
  GLADE_HOOKUP_OBJECT (hmgengui, colormap_scale2, "colormap_scale2");
  GLADE_HOOKUP_OBJECT (hmgengui, colormap_scale3, "colormap_scale3");
  GLADE_HOOKUP_OBJECT (hmgengui, colormap_scale4, "colormap_scale4");
  GLADE_HOOKUP_OBJECT (hmgengui, colormap_scale5, "colormap_scale5");
  GLADE_HOOKUP_OBJECT (hmgengui, colormap_scale6, "colormap_scale6");
  GLADE_HOOKUP_OBJECT (hmgengui, colormap_scale7, "colormap_scale7");
  GLADE_HOOKUP_OBJECT (hmgengui, colormap_scale8, "colormap_scale8");
  GLADE_HOOKUP_OBJECT (hmgengui, colormap_auto_update_checkbutton, "colormap_auto_update_checkbutton");
  GLADE_HOOKUP_OBJECT (hmgengui, colormap_update_button, "colormap_update_button");
  GLADE_HOOKUP_OBJECT (hmgengui, colormap_redraw_2dview_button, "colormap_redraw_2dview_button");
  GLADE_HOOKUP_OBJECT (hmgengui, export_format_combobox, "export_format_combobox");
  GLADE_HOOKUP_OBJECT (hmgengui, export_save_button, "export_save_button");
  GLADE_HOOKUP_OBJECT (hmgengui, export_filechooserwidget, "export_filechooserwidget");

  gtk_notebook_set_current_page(GTK_NOTEBOOK(main_notebook), 1);

  return hmgengui;
}

