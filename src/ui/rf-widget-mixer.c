#include <stdio.h>
#include <gtk/gtk.h>
#include "rf-widget-mixer.h"


GType
rf_mixer_get_type () {
	
	static GType rf_mixer_type = 0;

	if (!rf_mixer_type) {
		
		static const GTypeInfo ttt_info = {
			sizeof (RfMixerClass),
			NULL,
			NULL,
			(GClassInitFunc) rf_mixer_class_init,
			NULL,
			NULL,
			sizeof (RfMixer),
			0,
			(GInstanceInitFunc) rf_mixer_init
		};
		
		rf_mixer_type = g_type_register_static (GTK_TYPE_VBOX, "RfMixer", (GTypeInfo *) &ttt_info, 0);
	}

	return (rf_mixer_type);
}

static void
rf_mixer_class_init (RfMixerClass *class) {

	GObjectClass *object_class;
	object_class = (GObjectClass*) class;
	
	parent_class = g_type_class_peek_parent (class);

	
//	rf_mixer_signals[TICTACTOE_SIGNAL] = gtk_signal_new ("tictactoe",
//					GTK_RUN_FIRST,
//					G_TYPE_FROM_CLASS (object_class),
//					GTK_SIGNAL_OFFSET (RfMixerClass, tictactoe),
//					gtk_signal_default_marshaller,
//					GTK_TYPE_NONE, 0);
	//  gtk_object_class_add_signals (object_class, tictactoe_signals, LAST_SIGNAL);
//	class->tictactoe = NULL;

}

static void
rf_mixer_init (RfMixer *m) {
	
	GtkWidget *box;
	GtkWidget *frame;
	GtkWidget *button;

	frame = gtk_frame_new (NULL);
	gtk_container_set_border_width (GTK_CONTAINER (frame), 0);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_OUT);
	
	button = gtk_button_new_with_label ("plum");
	
  	m->priv = g_new0 (RfMixerPrivate, 1);
	m->priv->button = gtk_toggle_button_new ();
	
	box = gtk_vbox_new (FALSE, 1);
	//gtk_container_add (GTK_CONTAINER (ttt), box);
	//gtk_widget_show (box);
	gtk_container_add (GTK_CONTAINER (m), m->priv->button);
	
	g_signal_connect_object (G_OBJECT (m->priv->button), "toggled",
			 G_CALLBACK (rf_mixer_toggle),
			 m, 0);

	gtk_widget_show (m->priv->button);
	
	m->priv->window = gtk_window_new (GTK_WINDOW_POPUP);
	m->priv->scale = gtk_vscale_new_with_range (0, 100, 1);
	gtk_widget_set_size_request (m->priv->scale, -1, 100);


	gtk_container_add (GTK_CONTAINER (m->priv->window), frame);
	gtk_container_add (GTK_CONTAINER (frame), m->priv->scale);


}

GtkWidget *
rf_mixer_new () {
	
	return g_object_new ((rf_mixer_get_type ()), NULL);
	
}

static void
rf_mixer_toggle (GtkWidget *button, RfMixer *t) {

	gint x, y; /* odleglosc od rogu okienka */
	gint button_w, button_h; /* odleglosc od rogu ekranu */
	gint ww, wh;  /* rozmiar czegostam */

	gtk_widget_show_all (t->priv->window);
	gdk_window_get_origin (gtk_widget_get_parent_window (t->priv->button), &x, &y);
	gdk_drawable_get_size (gtk_widget_get_parent_window (t->priv->button), &button_w, &button_h);
	gdk_drawable_get_size (gtk_widget_get_parent_window (GTK_BIN (t->priv->window)->child), &ww, &wh);
	
	g_printf ("x:%d y:%d\nbutton_w:%d button_h:%d\nww: %d wh: %d\n", x, y, button_w, button_h, ww, wh);
	gint vx, vy;
	vx = x + (button_w - ww);
//	vy = bh + y - wh - y;
	vy = y + button_h - wh;

	g_printf ("vy:%d\n", vy);
	gtk_window_move (GTK_WINDOW (t->priv->window), vx, vy);

	g_printf ("Jednokliked!\n");

	    //GTK_TOGGLE_BUTTON(ttt->buttons[rwins[k][i]][cwins[k][i]])->active;
	  //gtk_signal_emit (GTK_OBJECT (ttt), 
//			   tictactoe_signals[TICTACTOE_SIGNAL]);
}
