/*
 * Copyright (C) 2004 £ukasz ¯ukowski <pax@legar.pl>
 *                    Piotr Tarasewicz <ptw@hurd.pl>
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
#include <gdk/gdkkeysyms.h>
#include "../rafesia.h"
#include "../core/internal.h"
#include "../plugins/plugins.h"
#include "rf-widget-mixer.h"

MediaModule *get_mediamodule (void);

MediaModule        *mediamod;
static gint         step = 5000;            // step in miliseconds
static int          no_recursion;
gboolean            fullscreen;

void
rf_interface_labelplaying_update (gchar *mrl) {
	
	GtkWidget   *lab = rf_widget_get ("rf playing file label");
	gchar       *file = mrl;
	gint         i = 0;
	
	if ( lab == NULL)
		return;
	
	for (; mrl[i]!='\0'; i++) {
		if(mrl[i] == '/' || mrl[i] == '\\')
			file=&mrl[i+1];
	}

	gtk_label_set_text (GTK_LABEL (lab), file);

}

void
rf_media_open_mrl (gchar *mrl, MediaModule *mmod) {

	gint       i=0;
	GtkWidget *img = rf_widget_get ("rf play image");
	
	mmod->open_mrl (mmod, mrl);
	mmod->play (mmod);

	if (img != NULL)
		gtk_image_set_from_stock (GTK_IMAGE (img), "gtk-media-pause", GTK_ICON_SIZE_BUTTON);

	rf_interface_labelplaying_update (mrl);
	
}

void
rf_media_play (GtkButton *button, MediaModule *mmod) {

	gint          status = mediamod->get_status (mediamod);
	GtkWidget    *img = rf_widget_get ("rf play image");
	
	switch (status) {
		case RF_STATUS_STOP:
		case RF_STATUS_PAUSE: 
			mediamod->play(mediamod);

			if (img != NULL)
				gtk_image_set_from_stock (GTK_IMAGE (img), "gtk-media-pause", GTK_ICON_SIZE_BUTTON);
			
			break;
		case RF_STATUS_PLAY:
			mediamod->pause(mediamod);
			
			if (img != NULL)
				gtk_image_set_from_stock (GTK_IMAGE (img), "gtk-media-play", GTK_ICON_SIZE_BUTTON);
			
			break;
	}

}

void
rf_interface_set_time_label (gint pos_time, gint length_time) {
	
	GtkWidget         *widget = rf_widget_get ("rf time label");
	gchar             *curtime_str, *length_str;
	gchar              time_str[256];
	gint               cur_time, length;
	
	if ( widget == NULL )
		return;
	
	cur_time = pos_time / 1000;
	length   = length_time / 1000;

	curtime_str = int_to_timestring (cur_time, 256);
	length_str  = int_to_timestring (length, 256);
	
	snprintf (time_str, 256, "%s / %s", curtime_str, length_str);
	gtk_label_set_text (GTK_LABEL (widget), time_str);

}

gboolean
update_slider_cb (gpointer seek) {

	gint           pos_stream, pos_time, length_time;
	GtkObject     *seeker = GTK_OBJECT(seek);
	GtkWidget     *img = rf_widget_get ("rf play image");
	gfloat         pos;

	if (mediamod->get_status (mediamod) != RF_STATUS_PLAY) {
		
		if (img != NULL)
			gtk_image_set_from_stock (GTK_IMAGE (img), "gtk-media-play", GTK_ICON_SIZE_BUTTON);

		return (TRUE);
		
	}
	
//	if (img != NULL)
//		gtk_image_set_from_stock (GTK_IMAGE (img), "gtk-media-pause", GTK_ICON_SIZE_BUTTON);
	
	if (!mediamod->get_position (mediamod, &pos_stream, &pos_time, &length_time))
		return (TRUE);

	pos = (gfloat) pos_stream;
	
	no_recursion = 1;
	gtk_adjustment_set_value (GTK_ADJUSTMENT (seeker), pos); 
	no_recursion = 0;
	gtk_adjustment_set_value (GTK_ADJUSTMENT (seeker), pos); 
	
	rf_interface_set_time_label(pos_time, length_time);
	
	return (TRUE);
	
}

static void
seek_cb (GtkWidget* widget, MediaModule *mmod) {

	gint          speed = mmod->get_status (mmod);
	GtkWidget    *img = rf_widget_get ("rf play image");
	
	if (!no_recursion ) {
		mmod->go (mmod, (gint) GTK_ADJUSTMENT(widget)->value, 0, 0); 
		if (speed == RF_STATUS_PAUSE && img != NULL)
			gtk_image_set_from_stock (GTK_IMAGE (img), "gtk-media-pause", GTK_ICON_SIZE_BUTTON);
	}
	
}

void
rf_media_go_backward (GtkButton *button, MediaModule *mmod) {
	
	GtkWidget *img = rf_widget_get ("rf play image");
	
	mmod->go (mmod, 0, 0-step, 1);
	
	if (img != NULL)
		gtk_image_set_from_stock (GTK_IMAGE (img), "gtk-media-pause", GTK_ICON_SIZE_BUTTON);
	
}

void
rf_media_go_forward (GtkButton *button, MediaModule *mmod) {

	GtkWidget *img = rf_widget_get ("rf play image");

	mmod->go (mmod, 0, step, 1);

	if (img != NULL)
		gtk_image_set_from_stock (GTK_IMAGE (img), "gtk-media-pause", GTK_ICON_SIZE_BUTTON);
	
}

gboolean
rf_fullscreen (GtkWidget *widget, MediaModule *mmod) { 
	
	if (fullscreen) {
	
		GtkWidget *widget2;
		/*GtkWidget *box = rf_widget_get ("(fullscreen) bottom_vbox");
		GtkWidget *window = rf_widget_get ("(fullscreen) bottom_window");
		
		gtk_widget_hide (window);*/
		
		widget2 = rf_widget_get ("rf menubar top");
		if ( widget2 != NULL)
			gtk_widget_show (widget2);
		
		widget2 = rf_widget_get ("rf box label");
		if ( widget2 != NULL ) {
		
			/*GtkWidget *vbox = rf_widget_get ("rf vbox main");
			
			g_object_ref (widget2);
			gtk_container_remove (GTK_CONTAINER (box), widget2);
			gtk_box_pack_start (GTK_BOX (vbox), widget2, FALSE, FALSE, 0);
		 	*/
			gtk_widget_show (widget2);
			
		}
		
		widget2 = rf_widget_get ("rf box bottom");
		if ( widget2 != NULL ) {
			/*
			GtkWidget *vbox = rf_widget_get ("rf vbox main");

			g_object_ref (widget2);
			gtk_container_remove (GTK_CONTAINER (box), widget2);
			gtk_box_pack_start (GTK_BOX (vbox), widget2, FALSE, FALSE, 0);
			*/
			gtk_widget_show (widget2);
			
		}
		
		widget2 = rf_widget_get ("rf main window");
		if ( widget2 != NULL )
			gtk_window_unfullscreen (GTK_WINDOW (widget2));
		
		fullscreen = FALSE;
		
	} else {
	
		GtkWidget *widget2;
		/*GtkWidget *window = rf_widget_get ("(fullscreen) bottom_window");
		GtkWidget *vbox = rf_widget_get ("(fullscreen) bottom_vbox");\
		gint       y = 0;*/
		
		widget2 = rf_widget_get ("rf menubar top");
		if ( widget2 != NULL )
			gtk_widget_hide (widget2);
  
		widget2 = rf_widget_get ("rf box label");
		if ( widget2 != NULL ) {
			
			/*GtkWidget *box = rf_widget_get ("rf vbox main");
		
			y = box->allocation.height;
			g_object_ref (widget2);
			gtk_container_remove (GTK_CONTAINER (box), widget2);
			gtk_box_pack_start (GTK_BOX (vbox), widget2, FALSE, FALSE, 0);
			*/
			gtk_widget_hide (widget2);
			
		}
		
		widget2 = rf_widget_get ("rf box bottom");
		if ( widget2 != NULL ) {
		
			/*GtkWidget *box = rf_widget_get ("rf vbox main");

			y = y + box->allocation.height;	
			g_object_ref (widget2);
			gtk_container_remove (GTK_CONTAINER (box), widget2);
			gtk_box_pack_start (GTK_BOX (vbox), widget2, FALSE, FALSE, 0);
		 	*/
			gtk_widget_hide (widget2);
		}
		
		widget2 = rf_widget_get ("rf main window");
		if ( widget2 != NULL )
			gtk_window_fullscreen (GTK_WINDOW (widget2));
		
		/*gtk_window_resize (GTK_WINDOW (window), gdk_screen_width (), y);
		gtk_window_move (GTK_WINDOW (window), 0, gdk_screen_height () - y);
		gtk_widget_show (window);*/
		fullscreen = TRUE;
		
	}
	
	return fullscreen;

}

gboolean
rf_file_open_cb (GtkWidget *widget, GdkEvent *event) {

	GtkWidget *dialog;

	dialog = gtk_file_chooser_dialog_new ("Open File",
					      NULL,
					      GTK_FILE_CHOOSER_ACTION_OPEN,
					      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
					      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
					      NULL);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
		gchar *filename;

		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		
		rf_media_open_mrl(filename, (MediaModule *)get_mediamodule ());
		rf_interface_labelplaying_update (filename);
		
		g_free (filename);
	}

	gtk_widget_destroy (dialog);

	return (TRUE);
	
}
	

gboolean
rf_window_aboutbox_cb (GtkWidget *widget, GdkEvent *event) {


	rf_window_about_build (NULL);

/*	GtkWidget *ab;
	g_printf ("kwa\n");

	ab = g_new0 (GtkWidget, sizeof(*GtkWidget));
	ab = gtk_about_dialog_new ();

	gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (ab), VERSION);
	gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (ab), "(c) Rafesia developers");
	gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (ab), "http://poklikaubym.pl");
	
	gtk_show_about_dialog (NULL, NULL);

*/	return (TRUE);

}


MediaModule *
get_mediamodule (void) {

	return (mediamod);
	
}

gboolean
rf_media_widget_motion_event (GtkWidget *widget, GdkEventMotion *event, gpointer user_data) {
	
	gint x, y, mask;
	GdkModifierType mods;

	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (event != NULL, FALSE);
	
	if (fullscreen) {
		GtkWidget *box       = rf_widget_get ("rf box bottom");
		GtkWidget *box_label = rf_widget_get ("rf box label");
		
		x = event->x;
		y = event->y;
		
		if ( box != NULL || box_label != NULL) {
			gint wy = gdk_screen_height () - (box->allocation.height + box_label->allocation.height);
			
			if (GTK_WIDGET_VISIBLE (box)) {
				if (wy > y) {
					gtk_widget_hide (box);
					gtk_widget_hide (box_label);
				}
			} else {
				if (wy < y) {
					gtk_widget_show (box);
					gtk_widget_show (box_label);
				}
			}
		}
		
		if (event->is_hint || (event->window != widget->window))
			gdk_window_get_pointer (widget->window, &x, &y, &mods);
	
	}
	
	return (TRUE);	

}

gboolean
rf_media_widget_key_press_event (GtkWidget *widget, GdkEventKey *event, gpointer user_data) {
	
	GdkEvent eventk;
	
	eventk.type = GDK_KEY_PRESS;
	eventk.key = *event;
	
	gtk_widget_event (GTK_WIDGET (user_data), &eventk);
	
}

gboolean
rf_media_widget_button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
	
	if (event->type == GDK_2BUTTON_PRESS && event->button == 1)
			rf_fullscreen (NULL, NULL);
		
}

gboolean
rf_media_widget_event (GtkWidget *widget, GdkEvent *event, gpointer user_data) {

	switch (event->type) {
		case (GDK_SCROLL): {
			switch (event->scroll.direction) {
				case (GDK_SCROLL_UP): rf_media_go_forward (NULL, get_mediamodule ()); break;
				case (GDK_SCROLL_DOWN): rf_media_go_backward (NULL, get_mediamodule ()); break;
				case (GDK_MOTION_NOTIFY): {
						g_printf ("GDK_MOTION_NOTIFY x: %f y: %f [xroot: %f, yroot: %f]\n", event->motion.x, event->motion.y, event->motion.x_root, event->motion.y_root);
						break;

				}
			}
		}
		case (GDK_NOTHING): g_printf ("GDK_NOTHING\n"); break;
		case (GDK_DELETE): g_printf ("GDK_DELETE\n"); break;
		case (GDK_DESTROY): g_printf ("GDK_DESTROY\n"); break;
		case (GDK_EXPOSE): g_printf ("GDK_EXPOSE\n"); break;
		case (GDK_BUTTON_PRESS): g_printf ("GDK_BUTTON_PRESS\n"); break;
		case (GDK_2BUTTON_PRESS): g_printf ("GDK_2BUTTON_PRESS\n"); break;
		case (GDK_3BUTTON_PRESS): g_printf ("GDK_3BUTTON_PRESS\n"); break;
		case (GDK_BUTTON_RELEASE): g_printf ("GDK_BUTTON_RELEASE\n"); break;
		case (GDK_KEY_PRESS): g_printf ("GDK_KEY_PRESS\n"); break;
		case (GDK_KEY_RELEASE): g_printf ("GDK_KEY_RELEASE\n"); break;
		case (GDK_ENTER_NOTIFY): g_printf ("GDK_ENTER_NOTIFY\n"); break;
		case (GDK_LEAVE_NOTIFY): g_printf ("GDK_LEAVE_NOTIFY\n"); break;
		case (GDK_FOCUS_CHANGE): g_printf ("GDK_FOCUS_CHANGE\n"); break;
		case (GDK_CONFIGURE): g_printf ("GDK_CONFIGURE\n"); break;
		case (GDK_MAP): g_printf ("GDK_MAP\n"); break;
		case (GDK_UNMAP): g_printf ("GDK_UNMAP\n"); break;
		case (GDK_PROPERTY_NOTIFY): g_printf ("GDK_PROPERTY_NOTIFY\n"); break;
		case (GDK_SELECTION_CLEAR): g_printf ("GDK_SELECTION_CLEAR\n"); break;
		case (GDK_SELECTION_REQUEST): g_printf ("GDK_SELECTION_REQUEST\n"); break;
		case (GDK_SELECTION_NOTIFY): g_printf ("GDK_SELECTION_NOTIFY\n"); break;
		case (GDK_PROXIMITY_IN): g_printf ("GDK_PROXIMITY_IN\n"); break;
		case (GDK_PROXIMITY_OUT): g_printf ("GDK_PROXIMITY_OUT\n"); break;
		case (GDK_DRAG_MOTION): g_printf ("GDK_DRAG_MOTION\n"); break;
		case (GDK_DRAG_LEAVE): g_printf ("GDK_DRAG_LEAVE\n"); break;
		case (GDK_DRAG_STATUS): g_printf ("GDK_DRAG_STATUS\n"); break;
		case (GDK_DROP_START): g_printf ("GDK_DROP_START\n"); break;
		case (GDK_DROP_FINISHED): g_printf ("GDK_DROP_FINISHED\n"); break;
		case (GDK_CLIENT_EVENT): g_printf ("GDK_CLIENT_EVENT\n"); break;
		case (GDK_VISIBILITY_NOTIFY): g_printf ("GDK_VISIBILITY_NOTIFY\n"); break;
		case (GDK_NO_EXPOSE): g_printf ("GDK_NO_EXPOSE\n"); break;
		case (GDK_WINDOW_STATE): g_printf ("GDK_WINDOW_STATE\n"); break;
		case (GDK_SETTING): g_printf ("GDK_SETTING\n"); break;
	
	}

	return (TRUE); 
}

GtkWidget *
rf_interface_main_window_create (MediaModule *mmod) {

	GtkWidget        *window;
	GtkWidget        *vbox1;

	GtkWidget        *menubar;
	GtkWidget        *menuitem_rafesia;
	GtkWidget        *menu1;
	GtkWidget        *menuitem_help;
	GtkWidget        *help1;
	GtkWidget        *menuitem_film;
	GtkWidget        *menu_film;
	
	
	GtkWidget        *menuFilm_fullscreen;
	GtkWidget        *menuFilm_play;
	GtkWidget        *menuFilm_back;
	GtkWidget        *menuFilm_forward;
	GtkWidget        *menuRafesia_open;
	GtkWidget        *menuSeparator;
	GtkWidget        *menuRafesia_quit;

	GtkWidget        *menuHelp_about;
	GtkWidget        *menuHelp_about_icon;

	GtkWidget        *eventbox_media;

	GtkWidget        *hbox1;
	GtkWidget        *labelPlaying;
	GtkWidget        *buttonAbout;
	GtkWidget        *labelAbout;
	GtkWidget        *labelSpace;
	GtkWidget        *labelTime;

	GtkWidget        *hbox2;

	GtkWidget        *alignmentBack;
	GtkWidget        *buttonBack;
	GtkWidget        *imageBack;

	GtkWidget        *buttonPlay;
	GtkWidget        *imagePlay;

	GtkWidget        *alignmentForward;
	GtkWidget        *buttonForward;
	GtkWidget        *imageForward;

	GtkWidget        *scaleTime;
	GtkObject        *seekerTime;

	GtkWidget        *alignmentVolume;
	GtkWidget        *buttonVolume;
	GtkWidget        *imageVolume;

	GtkAccelGroup    *accel_group;
	
	GtkWidget        *fullscreen_bottom_window;
	GtkWidget        *fullscreen_bottom_vbox;
	GtkWidget        *tmp_wdg;
	gint              y = 0;
	
	g_return_val_if_fail (mmod != NULL, (GtkWidget *)-1);
	accel_group = gtk_accel_group_new ();
	
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), ("Rafesia Movie Player"));
	rf_widget_add (window, "rf main window");

	vbox1 = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox1);
	gtk_container_add (GTK_CONTAINER (window), vbox1);
	rf_widget_add (vbox1, "rf vbox main");

	menubar = gtk_menu_bar_new ();
	gtk_widget_show (menubar);
	gtk_box_pack_start (GTK_BOX (vbox1), menubar, FALSE, FALSE, 0);
	rf_widget_add (menubar, "rf menubar top");
	
	menuitem_rafesia = gtk_menu_item_new_with_mnemonic ("_File");
	gtk_widget_show (menuitem_rafesia);
	gtk_container_add (GTK_CONTAINER (menubar), menuitem_rafesia);

	menu1 = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem_rafesia), menu1);


	menuitem_film = gtk_menu_item_new_with_mnemonic ("_View");
	gtk_widget_show (menuitem_film);
	gtk_container_add (GTK_CONTAINER (menubar), menuitem_film);

	menu_film = gtk_menu_new();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem_film), menu_film);
		
	
	menuitem_help = gtk_menu_item_new_with_mnemonic ("_Help");
	gtk_widget_show (menuitem_help);
	gtk_container_add (GTK_CONTAINER (menubar), menuitem_help);

	help1 = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem_help), help1);

	menuHelp_about = gtk_image_menu_item_new_with_mnemonic ("_About");
	menuHelp_about_icon = gtk_image_new_from_stock ("gtk-help", GTK_ICON_SIZE_MENU);
	gtk_widget_show (menuHelp_about_icon);
	gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menuHelp_about), menuHelp_about_icon);


	gtk_widget_show (menuHelp_about);
	gtk_container_add (GTK_CONTAINER (help1), menuHelp_about);
	
	menuRafesia_open = gtk_image_menu_item_new_from_stock ("gtk-open", accel_group);
	gtk_widget_show (menuRafesia_open);
	gtk_container_add (GTK_CONTAINER (menu1), menuRafesia_open);

	menuSeparator = gtk_separator_menu_item_new ();
	gtk_widget_show (menuSeparator);
	gtk_container_add (GTK_CONTAINER (menu1), menuSeparator);
	gtk_widget_set_sensitive (menuSeparator, FALSE);

	menuRafesia_quit = gtk_image_menu_item_new_from_stock ("gtk-quit", accel_group);
	gtk_widget_show (menuRafesia_quit);
	gtk_container_add (GTK_CONTAINER (menu1), menuRafesia_quit);

	menuFilm_play = gtk_image_menu_item_new_with_label ("Play/Pause");
	gtk_widget_show (menuFilm_play);
	gtk_container_add (GTK_CONTAINER (menu_film), menuFilm_play);
	
	menuFilm_back = gtk_image_menu_item_new_with_label ("Backward");
	gtk_widget_show (menuFilm_back);
	gtk_container_add (GTK_CONTAINER (menu_film), menuFilm_back);

	menuFilm_forward = gtk_image_menu_item_new_with_label ("Forward");
	gtk_widget_show (menuFilm_forward);
	gtk_container_add (GTK_CONTAINER (menu_film), menuFilm_forward);
	
	menuFilm_fullscreen =gtk_image_menu_item_new_with_label ("Fullscreen");
	gtk_widget_show (menuFilm_fullscreen);
	gtk_container_add (GTK_CONTAINER (menu_film), menuFilm_fullscreen);
	
	if ( mmod->widget != NULL) {
	
		gtk_container_add (GTK_CONTAINER (vbox1), mmod->widget);
		rf_widget_add (mmod->widget, "rf media widget");
		g_signal_connect (G_OBJECT (mmod->widget), "motion-notify-event", G_CALLBACK (rf_media_widget_motion_event), NULL);
		g_signal_connect (G_OBJECT (mmod->widget), "key-press-event", G_CALLBACK (rf_media_widget_key_press_event), window);
		g_signal_connect (G_OBJECT (mmod->widget), "button-press-event", G_CALLBACK (rf_media_widget_button_press_event), NULL);
		// g_signal_connect (G_OBJECT (mmod->widget), "event", G_CALLBACK (rf_media_widget_event), NULL);
		// ?? g_signal_connect (G_OBJECT (mmod->widget), "can-activate-accel", G_CALLBACK (rf_media_widget_activate_accel), NULL);
	
	}

	hbox1 = gtk_hbox_new (FALSE, 5);
	gtk_widget_show (hbox1);
	gtk_box_pack_start (GTK_BOX (vbox1), hbox1, FALSE, FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox1), 5);
	rf_widget_add (hbox1, "rf box label");
	
	labelPlaying = gtk_label_new ("");
	gtk_widget_show (labelPlaying);
	gtk_box_pack_start (GTK_BOX (hbox1), labelPlaying, FALSE, FALSE, 0);
	rf_widget_add (labelPlaying, "rf playing file label");

/*
	buttonAbout = gtk_button_new();
	gtk_button_set_relief(GTK_BUTTON(buttonAbout), GTK_RELIEF_NONE);
	gtk_widget_show(buttonAbout);
	gtk_box_pack_start (GTK_BOX (hbox1), buttonAbout, FALSE, FALSE, 0);
	
	labelAbout = gtk_label_new ("");
	gtk_label_set_markup(GTK_LABEL(labelAbout), "<span underline=\"single\" foreground=\"#0000FF\">(o filmie)</span>");
	gtk_widget_show (labelAbout);
	gtk_container_add (GTK_CONTAINER(buttonAbout), labelAbout);
 */
	
	labelSpace = gtk_label_new ("");
	gtk_widget_show (labelSpace);
	gtk_box_pack_start (GTK_BOX (hbox1), labelSpace, TRUE, TRUE, 0);

	labelTime = gtk_label_new ("0:00:00 / 0:00:00");
	gtk_widget_show (labelTime);
	gtk_box_pack_start (GTK_BOX (hbox1), labelTime, FALSE, FALSE, 0);
	rf_widget_add (labelTime, "rf time label");


	hbox2 = gtk_hbox_new (FALSE, 5);
	gtk_widget_show (hbox2);
	gtk_box_pack_start (GTK_BOX (vbox1), hbox2, FALSE, FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox2), 5);
	rf_widget_add (hbox2, "rf box bottom");

	alignmentBack = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignmentBack);
	gtk_box_pack_start (GTK_BOX (hbox2), alignmentBack, FALSE, FALSE, 0);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignmentBack), 3, 3, 3, 3);

	buttonBack = gtk_button_new ();
	gtk_widget_show (buttonBack);
	gtk_container_add (GTK_CONTAINER (alignmentBack), buttonBack);
	
	imageBack = gtk_image_new_from_stock ("gtk-media-rewind", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show (imageBack);
	gtk_container_add (GTK_CONTAINER (buttonBack), imageBack);


	buttonPlay = gtk_button_new ();
	gtk_widget_show (buttonPlay);
	gtk_box_pack_start (GTK_BOX (hbox2), buttonPlay, FALSE, FALSE, 0);

	imagePlay = gtk_image_new_from_stock ("gtk-media-play", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show (imagePlay);
	gtk_container_add (GTK_CONTAINER (buttonPlay), imagePlay);
	gtk_misc_set_padding (GTK_MISC (imagePlay), 6, 6);
	rf_widget_add (imagePlay, "rf play image");

	alignmentForward = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignmentForward);
	gtk_box_pack_start (GTK_BOX (hbox2), alignmentForward, FALSE, FALSE, 0);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignmentForward), 3, 3, 3, 3);

	buttonForward = gtk_button_new ();
	gtk_widget_show (buttonForward);
	gtk_container_add (GTK_CONTAINER (alignmentForward), buttonForward);

	imageForward = gtk_image_new_from_stock ("gtk-media-forward", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show (imageForward);
	gtk_container_add (GTK_CONTAINER (buttonForward), imageForward);


	seekerTime = gtk_adjustment_new (0.0, 0.0, 65536.0, 1.0, 10.0, 1.0);
	scaleTime = gtk_hscale_new (GTK_ADJUSTMENT (seekerTime));
	gtk_widget_show (scaleTime);
	gtk_box_pack_start (GTK_BOX (hbox2), scaleTime, TRUE, TRUE, 0);
	gtk_scale_set_draw_value (GTK_SCALE (scaleTime), FALSE);
	rf_widget_add ( (GtkWidget *)seekerTime, "rf media progress seeker");

	alignmentVolume = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignmentVolume);
	gtk_box_pack_start (GTK_BOX (hbox2), alignmentVolume, FALSE, FALSE, 0);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignmentVolume), 3, 3, 3, 3);
	
	//imageVolume = gtk_image_new_from_stock ("gtk-index", GTK_ICON_SIZE_BUTTON);
	//gtk_widget_show (imageVolume);
	buttonVolume = rf_mixer_new ();
	gtk_widget_show (buttonVolume);
	gtk_container_add (GTK_CONTAINER (alignmentVolume), buttonVolume);
	
	
	// Fullscreen windows
	/*fullscreen_bottom_window = gtk_window_new (GTK_WINDOW_POPUP);
	rf_widget_add (fullscreen_bottom_window, "(fullscreen) bottom_window");
	fullscreen_bottom_vbox = gtk_vbox_new (FALSE, 0);
	rf_widget_add (fullscreen_bottom_vbox, "(fullscreen) bottom_vbox");

	tmp_wdg = rf_widget_get ("rf box label");
	y = tmp_wdg->allocation.height;
	tmp_wdg = rf_widget_get ("rf box bottom");
	y += tmp_wdg->allocation.height;
	
	gtk_container_add (GTK_CONTAINER (fullscreen_bottom_window), fullscreen_bottom_vbox);
	gtk_widget_show (fullscreen_bottom_window);
	
	gtk_window_set_default_size (GTK_WINDOW (fullscreen_bottom_window), gdk_screen_width (), y);
	gtk_window_move (GTK_WINDOW (fullscreen_bottom_window), 0, gdk_screen_height () - y);*/
	
	// Signals
	g_signal_connect (GTK_OBJECT (window), "delete_event", G_CALLBACK (rafesia_quit),NULL);
	g_signal_connect (GTK_OBJECT (window), "destroy_event", G_CALLBACK (rafesia_quit),NULL);
	
	g_signal_connect (GTK_OBJECT (menuFilm_play), "activate", G_CALLBACK (rf_media_play), mmod);
	gtk_widget_add_accelerator (menuFilm_play, "activate", accel_group, GDK_space, 0, GTK_ACCEL_VISIBLE);
	
	g_signal_connect (GTK_OBJECT (menuFilm_fullscreen), "activate", G_CALLBACK (rf_fullscreen), mmod);
	gtk_widget_add_accelerator (menuFilm_fullscreen, "activate", accel_group, GDK_F, 0, GTK_ACCEL_VISIBLE);

	g_signal_connect (GTK_OBJECT (menuRafesia_open), "activate", G_CALLBACK (rf_file_open_cb), NULL);
	g_signal_connect (GTK_OBJECT (menuRafesia_quit), "activate", G_CALLBACK (rafesia_quit), NULL);
	
	g_signal_connect (GTK_OBJECT (menuHelp_about), "activate", G_CALLBACK (rf_window_aboutbox_cb), NULL);
	
	g_signal_connect (GTK_OBJECT (buttonPlay), "button-release-event", G_CALLBACK (rf_media_play), mmod);
	g_signal_connect (GTK_OBJECT (buttonPlay), "activate", G_CALLBACK (rf_media_play), mmod);
	
	g_signal_connect (GTK_OBJECT (buttonBack), "pressed", G_CALLBACK (rf_media_go_backward), mmod);
	g_signal_connect (GTK_OBJECT (buttonBack), "activate", G_CALLBACK (rf_media_go_backward), mmod);
	g_signal_connect (GTK_OBJECT (menuFilm_back), "activate", G_CALLBACK (rf_media_go_backward), mmod);
	gtk_widget_add_accelerator (menuFilm_back, "activate", accel_group, GDK_Left, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);
	
	g_signal_connect (GTK_OBJECT (buttonForward), "pressed", G_CALLBACK (rf_media_go_forward), mmod);
	g_signal_connect (GTK_OBJECT (buttonForward), "activate", G_CALLBACK (rf_media_go_forward), mmod);
	g_signal_connect (GTK_OBJECT (menuFilm_forward), "activate", G_CALLBACK (rf_media_go_forward), mmod);
	gtk_widget_add_accelerator (menuFilm_forward, "activate", accel_group, GDK_Right, GDK_MOD1_MASK, GTK_ACCEL_VISIBLE);
	
	g_signal_connect (GTK_OBJECT (seekerTime), "value-changed", G_CALLBACK (seek_cb), mmod);
	
	gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);

	mediamod = mmod;
	g_timeout_add (1000, update_slider_cb, seekerTime);

	return (window);

}

