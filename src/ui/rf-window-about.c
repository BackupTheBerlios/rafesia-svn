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
#include <gnome.h>

#include "../rafesia.h"
//#include <libgnome/libgnome.h>
//#include <libgnomeui-2.0/gnome.h>

GtkWidget *
rf_window_about_new (GtkWidget *parent) {

//	rf_ (NULL);

}

GtkWidget *
rf_window_about_build (GtkWidget *parent) {


//	GtkWidget *dialog;
//	dialog = gtk_about_dialog_new ();
//	exit (0);
//	exit (0);
	//gtk_show_about_dialog (parent, NULL);
	
//	gtk_widget_destroy (dialog);
//	GtkWidget *win;
//win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
//	gtk_widget_show (win);
//return (win);
	

//	GtkWidget *win;
//	const gchar *authors[] = {"Lukasz Zukowski", "Piotr Tarasewicz", NULL};
//	win = gnome_about_new ("Rafesia", "2.22", "aaaa", authors, authors, NULL, "dfdfd", NULL);
 /* 
	GtkWidget *dialog;
	GtkWidget *vbox;
	GtkWidget *label;

	dialog = gtk_dialog_new_with_buttons ("About", GTK_WINDOW (parent), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE, NULL);
	gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
	
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), vbox, FALSE, FALSE, 0);
	
	gchar *str;
	str = g_strdup_printf ("Rafesia %s", "0.0.1"); // FIXME na razie nie chce sie opierac na rafesia.h
	label = gtk_label_new (str);

	gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);

	gtk_widget_show_all (vbox);
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
*/
//  const gchar *authors[] = {"Luke Zukowski <pax@legar.pl>", "Piotr Tarasewicz <ptw@hurd.pl>", NULL };

/*	GdkPixbuf   *pixbuf = NULL;
    GError  	*error = NULL;
    GtkIconInfo *icon_info;
    
    const gchar *authors[] = {
        "Mike Hughes <mfh@psilord.com>",
        "Spiros Papadimitriou <spapadim+@cs.cmu.edu>",
        "Bradford Hovinen <hovinen@udel.edu>",
        NULL
    };
    gchar *documenters[] = {
	    NULL
    };
    gchar *translator_credits = _("translator_credits");
    GtkWidget *about;
    
    icon_info = gtk_icon_theme_lookup_icon (gtk_icon_theme_get_default (), "gdict", 48, 0);
    if (icon_info) {
        pixbuf = gtk_icon_info_load_icon (icon_info, &error);
        
        if (error) {
    	   g_warning (G_STRLOC ": cannot open %s: %s", gtk_icon_info_get_filename (icon_info), error->message);
	   g_error_free (error);	
        }
    }
    
    about = gnome_about_new (_("Dictionary"), VERSION,
                            "Copyright \xc2\xa9 1999-2003 Mike Hughes",
                            _("A client for the MIT dictionary server."),
			     (const char **)authors,
			     (const char **)documenters,
			     strcmp (translator_credits, "translator_credits") != 0 ? translator_credits : NULL,
                             NULL);
    if (pixbuf) {
    	   gdk_pixbuf_unref (pixbuf);
    }

    gnome_window_icon_set_from_file (GTK_WINDOW (about), gtk_icon_info_get_filename (icon_info));
    
    if (icon_info) {
    	gtk_icon_info_free (icon_info);
    }

    gtk_widget_show (about);
*/
}
