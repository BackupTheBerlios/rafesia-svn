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
#include "../rafesia.h"
#include "../core/internal.h"
#include "../plugins/plugins.h"
#include "rf-widget-mixer.h"

MediaModule *get_mediamodule (void);

MediaModule        *mediamod;
GtkWidget          *labelPlaying;
GtkWidget          *labelTime;
GtkWidget          *imagePlay;
static gint         step = 5000;            // step in miliseconds
static int          no_recursion;

GtkWidget          *window;
gboolean            fullscreen;

void
rf_interface_labelplaying_update (gchar *mrl) {

	gchar *file=mrl;
	gint i=0;

	for (; mrl[i]!='\0'; i++) {
		if(mrl[i] == '/' || mrl[i] == '\\')
			file=&mrl[i+1];
	}

	gtk_label_set_text (GTK_LABEL (labelPlaying), file);

}

void
rf_media_open_mrl (gchar *mrl, MediaModule *mmod) {

	int i=0;
	mmod->open_mrl (mmod, mrl);
	mmod->play (mmod);
	gtk_image_set_from_file (GTK_IMAGE (imagePlay), g_build_filename ("./images/", "pause.png", NULL));

	rf_interface_labelplaying_update (mrl);
	
}

void
rf_media_play (GtkButton *button, MediaModule *mmod) {

	gint status = mediamod->get_status (mediamod);

	switch (status) {
		case RF_STATUS_STOP:
		case RF_STATUS_PAUSE:
			mediamod->play(mediamod);
			gtk_image_set_from_file (GTK_IMAGE (imagePlay), g_build_filename ("./images/", "pause.png", NULL));
			break;
		case RF_STATUS_PLAY:
			mediamod->pause(mediamod);
			gtk_image_set_from_file(GTK_IMAGE (imagePlay), g_build_filename ("./images/", "play.png", NULL));
			break;
	}

}

void
rf_interface_set_time_label (gint pos_time, gint length_time) {

	gchar *curtime_str, *length_str;
	gchar time_str[256];
	gint cur_time, length;
	
	cur_time = pos_time / 1000;
	length   = length_time / 1000;

	curtime_str = int_to_timestring (cur_time, 256);
	length_str = int_to_timestring (length, 256);
	
	snprintf(time_str, 256, "%s / %s", curtime_str, length_str);
	gtk_label_set_text (GTK_LABEL (labelTime), time_str);

}

gboolean
update_slider_cb (gpointer seek) {

	gint pos_stream, pos_time, length_time;
	GtkObject *seeker = GTK_OBJECT(seek);
	gfloat pos;

	if (mediamod->get_status (mediamod) != RF_STATUS_PLAY) {
	
		gtk_image_set_from_file(GTK_IMAGE (imagePlay), g_build_filename ("./images/", "play.png", NULL));

		return (TRUE);
		
	}
	
	gtk_image_set_from_file(GTK_IMAGE (imagePlay), g_build_filename ("./images/", "pause.png", NULL));
	
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

	gint speed = mmod->get_status (mmod);

	if (!no_recursion ) {
		mmod->go (mmod, (gint) GTK_ADJUSTMENT(widget)->value, 0, 0); 
		if (speed == RF_STATUS_PAUSE)
			gtk_image_set_from_file (GTK_IMAGE (imagePlay), g_build_filename ("./images/", "pause.png", NULL));
	}
	
}

void
rf_media_go_backward (GtkButton *button, MediaModule *mmod) {

	mmod->go (mmod, 0, 0-step, 1);
	gtk_image_set_from_file (GTK_IMAGE (imagePlay), g_build_filename ("./images/", "pause.png", NULL));
	
}

void
rf_media_go_forward (GtkButton *button, MediaModule *mmod) {

	mmod->go (mmod, 0, step, 1);
	gtk_image_set_from_file (GTK_IMAGE (imagePlay), g_build_filename ("./images/", "pause.png", NULL));
	
}

gboolean
rf_fullscreen (GtkWidget *widget, MediaModule *mmod) { 
	
	if (fullscreen) {
		gtk_window_unfullscreen (GTK_WINDOW (window) );
		fullscreen = FALSE;
	} else {
		gtk_window_fullscreen ( GTK_WINDOW (window) );
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
		
		rf_media_open_mrl(filename, (MediaModule *)get_mediamodule());
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

GtkWidget *
rf_interface_main_window_create (MediaModule *mmod) {

	
	GtkWidget        *vbox1;

	GtkWidget        *menubar;
	GtkWidget        *menuitem_rafesia;
	GtkWidget        *menu1;
	GtkWidget        *menuitem_help;
	GtkWidget        *help1;
	GtkWidget        *menuitem_film;
	GtkWidget        *menu_film;
	
	
	GtkWidget        *menuFilm_fullscreen;
	GtkWidget        *menuRafesia_open;
	GtkWidget        *menuSeparator;
	GtkWidget        *menuRafesia_quit;

	GtkWidget        *menuHelp_about;
	GtkWidget        *menuHelp_about_icon;


	GtkWidget        *hbox1;
	GtkWidget        *buttonAbout;
	GtkWidget        *labelAbout;
	GtkWidget        *labelSpace;

	GtkWidget        *hbox2;

	GtkWidget        *alignmentBack;
	GtkWidget        *buttonBack;
	GtkWidget        *imageBack;

	GtkWidget        *buttonPlay;

	GtkWidget        *alignmentForward;
	GtkWidget        *buttonForward;
	GtkWidget        *imageForward;

	GtkWidget        *scaleTime;
	GtkObject        *seekerTime;

	GtkWidget        *alignmentVolume;
	GtkWidget        *buttonVolume;
	GtkWidget        *imageVolume;

	GtkAccelGroup    *accel_group;
	
	g_return_val_if_fail (mmod != NULL, (GtkWidget *)-1);
	accel_group = gtk_accel_group_new ();
	
	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title (GTK_WINDOW (window), ("Rafesia Movie Player"));

	vbox1 = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox1);
	gtk_container_add (GTK_CONTAINER (window), vbox1);

	menubar = gtk_menu_bar_new ();
	gtk_widget_show (menubar);
	gtk_box_pack_start (GTK_BOX (vbox1), menubar, FALSE, FALSE, 0);

	menuitem_rafesia = gtk_menu_item_new_with_mnemonic ("Rafesia");
	gtk_widget_show (menuitem_rafesia);
	gtk_container_add (GTK_CONTAINER (menubar), menuitem_rafesia);

	menu1 = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem_rafesia), menu1);


	menuitem_film = gtk_menu_item_new_with_mnemonic ("Film");
	gtk_widget_show (menuitem_film);
	gtk_container_add (GTK_CONTAINER (menubar), menuitem_film);

	menu_film = gtk_menu_new();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem_film), menu_film);
		
	
	menuitem_help = gtk_menu_item_new_with_mnemonic ("Help");
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

	menuFilm_fullscreen =gtk_image_menu_item_new_with_label ("Fullscreen");
	gtk_widget_show (menuFilm_fullscreen);
	gtk_container_add (GTK_CONTAINER (menu_film), menuFilm_fullscreen);
	
	if ( mmod->widget != NULL)
		gtk_container_add (GTK_CONTAINER (vbox1), mmod->widget);

	hbox1 = gtk_hbox_new (FALSE, 5);
	gtk_widget_show (hbox1);
	gtk_box_pack_start (GTK_BOX (vbox1), hbox1, FALSE, FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox1), 5);

	labelPlaying = gtk_label_new ("");
	gtk_widget_show (labelPlaying);
	gtk_box_pack_start (GTK_BOX (hbox1), labelPlaying, FALSE, FALSE, 0);

	buttonAbout = gtk_button_new();
	gtk_button_set_relief(GTK_BUTTON(buttonAbout), GTK_RELIEF_NONE);
	gtk_widget_show(buttonAbout);
	gtk_box_pack_start (GTK_BOX (hbox1), buttonAbout, FALSE, FALSE, 0);
	
	labelAbout = gtk_label_new ("");
	gtk_label_set_markup(GTK_LABEL(labelAbout), "<span underline=\"single\" foreground=\"#0000FF\">(o filmie)</span>");
	gtk_widget_show (labelAbout);
	gtk_container_add (GTK_CONTAINER(buttonAbout), labelAbout);

	labelSpace = gtk_label_new ("");
	gtk_widget_show (labelSpace);
	gtk_box_pack_start (GTK_BOX (hbox1), labelSpace, TRUE, TRUE, 0);

	labelTime = gtk_label_new ("0:00:00 / 0:00:00");
	gtk_widget_show (labelTime);
	gtk_box_pack_start (GTK_BOX (hbox1), labelTime, FALSE, FALSE, 0);



	hbox2 = gtk_hbox_new (FALSE, 5);
	gtk_widget_show (hbox2);
	gtk_box_pack_start (GTK_BOX (vbox1), hbox2, FALSE, FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox2), 5);

	alignmentBack = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignmentBack);
	gtk_box_pack_start (GTK_BOX (hbox2), alignmentBack, FALSE, FALSE, 0);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignmentBack), 3, 3, 3, 3);

	buttonBack = gtk_button_new ();
	gtk_widget_show (buttonBack);
	gtk_container_add (GTK_CONTAINER (alignmentBack), buttonBack);

	imageBack = gtk_image_new_from_file(g_build_filename("./images/", "rew.png", NULL));
	gtk_widget_show (imageBack);
	gtk_container_add (GTK_CONTAINER (buttonBack), imageBack);


	buttonPlay = gtk_button_new ();
	gtk_widget_show (buttonPlay);
	gtk_box_pack_start (GTK_BOX (hbox2), buttonPlay, FALSE, FALSE, 0);

	imagePlay = gtk_image_new_from_file(g_build_filename("./images/", "play.png", NULL));
	gtk_widget_show (imagePlay);
	gtk_container_add (GTK_CONTAINER (buttonPlay), imagePlay);
	gtk_misc_set_padding (GTK_MISC (imagePlay), 6, 6);


	alignmentForward = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignmentForward);
	gtk_box_pack_start (GTK_BOX (hbox2), alignmentForward, FALSE, FALSE, 0);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignmentForward), 3, 3, 3, 3);

	buttonForward = gtk_button_new ();
	gtk_widget_show (buttonForward);
	gtk_container_add (GTK_CONTAINER (alignmentForward), buttonForward);

	imageForward = gtk_image_new_from_file(g_build_filename("./images/", "fov.png", NULL));
	gtk_widget_show (imageForward);
	gtk_container_add (GTK_CONTAINER (buttonForward), imageForward);


	seekerTime = gtk_adjustment_new (0.0, 0.0, 65536.0, 1.0, 10.0, 1.0);
	scaleTime = gtk_hscale_new (GTK_ADJUSTMENT (seekerTime));
	gtk_widget_show (scaleTime);
	gtk_box_pack_start (GTK_BOX (hbox2), scaleTime, TRUE, TRUE, 0);
	gtk_scale_set_draw_value (GTK_SCALE (scaleTime), FALSE);


	alignmentVolume = gtk_alignment_new (0.5, 0.5, 1, 1);
	gtk_widget_show (alignmentVolume);
	gtk_box_pack_start (GTK_BOX (hbox2), alignmentVolume, FALSE, FALSE, 0);
	gtk_alignment_set_padding (GTK_ALIGNMENT (alignmentVolume), 3, 3, 3, 3);

	//imageVolume = gtk_image_new_from_file (g_build_filename ("./images/", "volume.png", NULL));
	//gtk_widget_show (imageVolume);
	buttonVolume = rf_mixer_new ();
	gtk_widget_show (buttonVolume);
	gtk_container_add (GTK_CONTAINER (alignmentVolume), buttonVolume);

	g_signal_connect (GTK_OBJECT (window), "delete_event", G_CALLBACK (rafesia_quit),NULL);
	g_signal_connect (GTK_OBJECT (window), "destroy_event", G_CALLBACK (rafesia_quit),NULL);
	
	g_signal_connect (GTK_OBJECT (menuFilm_fullscreen), "activate", G_CALLBACK (rf_fullscreen), mmod);
	g_signal_connect (GTK_OBJECT (menuRafesia_open), "activate", G_CALLBACK (rf_file_open_cb), NULL);
	g_signal_connect (GTK_OBJECT (menuRafesia_quit), "activate", G_CALLBACK (rafesia_quit), NULL);
	
	g_signal_connect (GTK_OBJECT (menuHelp_about), "activate", G_CALLBACK (rf_window_aboutbox_cb), NULL);
	
	g_signal_connect (GTK_OBJECT (buttonPlay), "button-release-event", G_CALLBACK (rf_media_play), mmod);
	g_signal_connect (GTK_OBJECT (buttonPlay), "activate", G_CALLBACK (rf_media_play), mmod);
	g_signal_connect (GTK_OBJECT (buttonBack), "pressed", G_CALLBACK (rf_media_go_backward), mmod);
	g_signal_connect (GTK_OBJECT (buttonBack), "activate", G_CALLBACK (rf_media_go_backward), mmod);
	g_signal_connect (GTK_OBJECT (buttonForward), "pressed", G_CALLBACK (rf_media_go_forward), mmod);
	g_signal_connect (GTK_OBJECT (buttonForward), "activate", G_CALLBACK (rf_media_go_forward), mmod);
	g_signal_connect (GTK_OBJECT (seekerTime), "value-changed", G_CALLBACK (seek_cb), mmod);

	mediamod = mmod;
	g_timeout_add (1000, update_slider_cb, seekerTime);
	
	return (window);

}

