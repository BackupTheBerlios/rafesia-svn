/*
 * Copyright (C) 2004 Piotr Tarasewicz <ptw@hurd.pl>
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

#include <stdio.h>
#include <glib.h>
#include <gtk/gtk.h>

#include "../rafesia.h"

GtkWidget *
rf_window_about_new (GtkWidget *parent) {

}

GtkWidget *
rf_window_about_build (GtkWidget *parent) {

	GtkWidget *dialog;
	GtkWidget *vbox;
	GtkWidget *label;
       	gchar *str_name;
	gchar *str_authors;
	gchar *str;
	
	gchar *authors[] = {"Łukasz Żukowski", "Piotr Tarasewicz", NULL};
        
	dialog = gtk_dialog_new_with_buttons ("About", GTK_WINDOW (parent), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE, NULL);
	gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
	
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), vbox, FALSE, FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER(vbox), 4);

	str_name = g_strdup_printf ("<span size=\"x-large\">Rafesia</span>\n<span size=\"small\">version %s</span>\n\n", VERSION);
	str_authors = g_strjoinv ("\n", authors);
	str = g_strjoin (NULL, str_name, str_authors);

	label = gtk_label_new (str);
	gtk_label_set_markup (GTK_LABEL (label), str);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_CENTER);
	gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);

	g_free (str_name);
	g_free (str_authors);
	g_free (str);

	gtk_widget_show_all (vbox);
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}
