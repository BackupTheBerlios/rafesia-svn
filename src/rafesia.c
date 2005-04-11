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


#include <gtk/gtk.h>
#include "rafesia.h"
#include <rafesia/plugins.h>
#include "ui/rf-window-main.h"

GMainLoop         *mainloop;
MediaModule       *mediamod;
GSList            *widgets_list=NULL;

gpointer
rf_log_handler (const gchar *log_domain, GLogLevelFlags log_level, const gchar *message, gpointer user_data) {

	gchar *type;
	gchar *color;
	
	switch (log_level) {
		case G_LOG_LEVEL_ERROR:
		case G_LOG_LEVEL_CRITICAL:
			type = g_strdup ("ERROR");
			color = RF_RED;
			break;
		case G_LOG_LEVEL_MESSAGE:
			type = g_strdup ("MESSAGE");
			color = RF_GREEN;
			break;
		case G_LOG_LEVEL_WARNING:
			type = g_strdup ("WARNING");
			color = RF_YELLOW;
			break;
		case G_LOG_LEVEL_DEBUG:
		case G_LOG_LEVEL_INFO:
			type = g_strdup ("DEBUG");
			color = RF_WHITE;
			break;
	};
	
	g_printf ("%s%7s%s: %s", color, type, RF_NORMAL, message);

}

GtkWidget *
rf_widget_get (gchar *name) {

	GSList *lista = widgets_list;

	while (lista != NULL) {
	
		gchar *wname;
		
		wname = g_object_get_data (G_OBJECT (lista->data), "rf_widget_name");
		
		if (g_ascii_strcasecmp (name, wname) == 0)
			return (GtkWidget *) lista->data;

		lista = g_slist_next (lista);
	
	}

	return NULL;

}

gboolean
rf_widget_remove (gchar *name) {
	
	GtkWidget *widget = rf_widget_get (name);

	if (widget == NULL) 
		return FALSE;
		
	widgets_list = g_slist_remove (widgets_list, widget);
	
	return TRUE;
	
}

gboolean
rf_widget_add (GtkWidget *widget, gchar *name) {

	if (rf_widget_get (name) != NULL)
		return FALSE;
	
	g_object_set_data (G_OBJECT (widget), "rf_widget_name", name);
	widgets_list = g_slist_append (widgets_list, widget);
	
	return TRUE;
	
}

void
scr_init (void) {
	
	g_message ("Rafesia - movie player v%s%s\n", RF_BOLD, VERSION);
	g_message ("(c) 2004 by Rafesia Team.\n");
	
}

void
rafesia_quit (void) {
	
	MediaModule *mmod = (MediaModule *) get_mediamodule();

	if (mmod != NULL)
		mmod->stop (mmod);
	
	g_main_loop_quit (mainloop);
	
}

void
rf_set_module_media (void *mmod) {

	mediamod = (MediaModule *) mmod;
	
}

void
media_event_cb (gint event) {
	
	switch (event) {
		case RF_EVENT_PLAYBACK_FINISHED: {
		
			MediaModule   *mmod = (MediaModule *) get_mediamodule();
			GtkWidget     *seek = rf_widget_get ("rf media progress seeker");
			gint           length_time;
			
			gtk_adjustment_set_value ( (GtkAdjustment *)seek, 0);
			mmod->go (mmod, 0, 0, 0);
			mmod->get_position (mmod, NULL, NULL, &length_time);
			mmod->stop (mmod);
			rf_interface_set_time_label (0, length_time);
		}
	}

}

gboolean
rf_gui_create (gpointer data) {

	gchar   *mrl = (gchar *) data;
	
	if (mediamod != NULL && g_main_loop_is_running (mainloop)) {
		GtkWidget         *win;
	
		win = rf_interface_main_window_create (mediamod);
		
		gdk_window_process_updates (gtk_widget_get_root_window(win), TRUE);
		
		gtk_widget_show_all (win);
		gtk_widget_hide (mediamod->widget);
		gtk_widget_show (mediamod->widget);
	
		mediamod->event_init (mediamod->widget, media_event_cb);
		
		if  (mrl != NULL)
			rf_media_open_mrl (mrl, mediamod);
		
		return (FALSE);
	}

	return (TRUE);
	
}

int
main (gint argc, gchar *argv[]) {

	GMainContext      *maincontext;
	gint               i;
	gchar             *mrl = NULL;

	/* set handler for all incoming messages */
	g_log_set_handler (NULL, G_LOG_LEVEL_ERROR | G_LOG_LEVEL_CRITICAL | G_LOG_LEVEL_WARNING |
			G_LOG_LEVEL_MESSAGE | G_LOG_LEVEL_INFO | G_LOG_LEVEL_DEBUG,
			(GLogFunc) rf_log_handler, NULL);
	
	scr_init ();

	if (!g_module_supported ())
		g_error ("Cannot initialize modules.\n");

	if (argc > 0)
		mrl = argv[1];
	
	XInitThreads ();

	g_thread_init (NULL);
	gdk_threads_init ();


	
	gtk_init (&argc, &argv);
	
	g_set_application_name ("Rafesia Movie Player");
	
	mediamod = rf_module_media_load (RF_LIBDIR, NULL);
	/*if (mediamod == NULL) {
		
		g_message ("No media module\n");
		return 0;
		
	}*/
	
	maincontext = g_main_context_default ();
	mainloop = g_main_loop_new (maincontext, FALSE);

	
	g_timeout_add (100, rf_gui_create, mrl);
	g_main_loop_run (mainloop);
	
	return (0);

}

