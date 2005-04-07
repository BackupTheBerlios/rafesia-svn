#include "rf-widget-mixer.h"

static GtkWidgetClass     *parent_class = NULL;

void
rf_mixer_activate (GtkButton *button, gpointer user_data) {
	
	GtkWidget *but    = GTK_WIDGET (button);
	RfMixer *rmix = RF_MIXER (button);
	gint x, y;
	gint button_w, button_h;
	
	rmix->window = gtk_window_new (GTK_WINDOW_POPUP);
	gtk_window_set_default_size (GTK_WINDOW (rmix->window), 28, 100);

	gtk_container_add (GTK_CONTAINER (rmix->window), rmix->scale);
	gtk_widget_show (rmix->scale);

	gdk_window_get_origin (GDK_WINDOW (but->window), &x, &y);
	
	gdk_drawable_get_size (gtk_widget_get_parent_window (but), &button_w, &button_h);
	gtk_window_move ( GTK_WINDOW (rmix->window), x+button_w-36, y+button_h-106);	
	
	gtk_widget_show (rmix->window);
	
}

gboolean
rf_mixer_button_release (GtkWidget *widget, GdkEventButton *event, gpointer user_data) {
	
	RfMixer *rmix = RF_MIXER (user_data);
	
	gtk_container_remove (GTK_CONTAINER (rmix->window), rmix->scale);
	gtk_widget_hide (rmix->scale);
	gtk_widget_destroy (GTK_WIDGET (rmix->window));
	rmix->window = NULL;

	return TRUE;

}

gboolean
rf_mixer_mouse_motion (GtkWidget *widget, GdkEventMotion *event, gpointer user_data) {
	
	gint ex, ey, x, y, w, h;
	RfMixer *rmix = RF_MIXER (user_data);
	
	ex = event->x;
	ey = event->y;
	gdk_window_get_origin (GDK_WINDOW (widget->window), &x, &y);
	gdk_drawable_get_size (gtk_widget_get_parent_window (widget), &w, &h);
	
	//if (event->x < x || event->y < y || event->x > x+w || event->y > y+h)
	//	gtk_widget_hide (GTK_WIDGET (user_data));
	
	//g_printf ("ex: %d ey: %d x: %d  y: %d wx: %d hy: %d\n", ex, ey, x, y, x+w, y+h);

	return TRUE;
}

static void 
rf_mixer_finalize (GObject *object) {

	G_OBJECT_CLASS (parent_class)->finalize (object);
	
}

static void
rf_mixer_init (RfMixer *rmix) {
	
	GtkWidget *but = GTK_WIDGET (rmix);
	GtkWidget *image = gtk_image_new_from_stock ("gtk-index", GTK_ICON_SIZE_BUTTON);
	
	gtk_container_add (GTK_CONTAINER (rmix), image);
	
	rmix->scale  = gtk_vscale_new_with_range (0, 100, 1);
	g_object_ref(rmix->scale);
	
	gtk_scale_set_draw_value (GTK_SCALE (rmix->scale), FALSE);

	g_signal_connect (GTK_OBJECT (rmix->scale), "button-release-event", G_CALLBACK (rf_mixer_button_release), rmix);
	//g_signal_connect (GTK_OBJECT (rmix), "activate", G_CALLBACK (rf_mixer_activate), rmix);
	//g_signal_connect (GTK_OBJECT (rmix), "clicked", G_CALLBACK (rf_mixer_activate), rmix);
	//g_signal_connect (GTK_OBJECT (rmix), "pressed", G_CALLBACK (rf_mixer_activate), rmix);

}

void
rf_mixer_class_init (gpointer class, gpointer class_data) {

	GObjectClass              *object_class;
	GtkButtonClass            *widget_class;
	
	object_class = (GObjectClass *) class;
	widget_class = (GtkButtonClass *) class;

	parent_class = gtk_type_class (gtk_button_get_type ());

	/* GObject */
	object_class->set_property             = NULL;
	object_class->get_property             = NULL;
	object_class->finalize                 = rf_mixer_finalize;

}

GType 
rf_mixer_get_type (void) {
	static GType type = 0;
	
	if (!type) {
		static const GTypeInfo info = {
			sizeof (RfMixerClass),
			NULL, /* base_init */
			NULL, /* base_finalize */
			(GClassInitFunc) rf_mixer_class_init,
			NULL, /* class_finalize */
			NULL, /* class_data */
			sizeof (RfMixer),
			0,
			(GInstanceInitFunc) rf_mixer_init,
		};
		
		type = g_type_register_static (GTK_TYPE_BUTTON, "RF_MIXER_TYPE", &info, 0);
	}
	
	return (type);
	
}

GtkWidget *
rf_mixer_new (void) {
	
	return GTK_WIDGET (g_object_new (rf_mixer_get_type (), NULL));
	
}
