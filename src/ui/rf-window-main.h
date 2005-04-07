/*
 * Copyright (C) 2004 £ukasz ¯ukowski <pax@legar.pl>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

GtkWidget *rf_interface_main_window_create (MediaModule *mmod);
void rf_interface_set_time_label (gint pos_time, gint length_time);
MediaModule *get_mediamodule (void);
void rf_media_open_mrl (gchar *mrl, MediaModule *mmod);
gboolean rf_file_open_cb (GtkWidget *widget, GdkEvent *event);
gboolean rf_window_aboutbox_cb (GtkWidget *widget, GdkEvent *event);

