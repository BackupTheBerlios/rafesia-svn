#include <gtk/gtk.h>
#include "rf-media-mplayer.h"

#include <gdk/gdkkeysyms.h>

#include <stdlib.h>
#include <unistd.h>
#include <gdk/gdkx.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>

static GtkWidgetClass *parent_class = NULL;

gboolean rf_media_mplayer_output_lookup (GIOChannel *source, GIOCondition condition, gpointer data);
gboolean rf_media_mplayer_timeout (gpointer data);

gboolean
rf_media_mplayer_is_running (GtkWidget *widget) {
	
	RfMediaMplayer  *rmm;
	
	g_return_if_fail (widget != NULL);
	g_return_if_fail (IS_RF_MEDIA_MPLAYER (widget));

	rmm = RF_MEDIA_MPLAYER (widget);
	
	if (!rmm->ready && rmm->mp_pid > 0)
		return TRUE;

	return FALSE;

}

gint
rf_media_mplayer_launch (GtkWidget *widget) {
	
	extern char       **environ;
	gchar              *command;
	RfMediaMplayer     *rmm = RF_MEDIA_MPLAYER (widget);
	gchar              *argv[] = {"mplayer", "-wid", g_strdup_printf ("%d", GDK_WINDOW_XWINDOW (widget->window)), "-identify", "-vo", "xv", "-slave", "-osdlevel", "0", "-nolirc", rmm->file, '\0'};
//	gchar              *argv[] = {"mplayer", "-wid", g_strdup_printf ("%d", GDK_WINDOW_XWINDOW (widget->window)), "-identify", "-vo", "xv", "-slave", "-osdlevel", "0", "-nolirc", "-noautosub", rmm->file, '\0'};

	if (!g_spawn_async_with_pipes (NULL, argv, NULL, G_SPAWN_SEARCH_PATH | G_SPAWN_STDERR_TO_DEV_NULL, NULL, NULL, &(rmm->mp_pid), &(rmm->mp_in), &(rmm->mp_out), NULL, NULL))
		return -1;
	
	g_free (argv[2]);

	rmm->channel_output   = g_io_channel_unix_new (rmm->mp_out);
	rmm->stream_input     = fdopen (rmm->mp_in, "w");
	rmm->channel_input    = g_io_channel_unix_new (rmm->mp_in);
			
	rmm->width            = 0;
	rmm->height           = 0;
	rmm->length           = 0;
	rmm->timer            = 0;
	rmm->ready            = FALSE;

	while ((rmm->width == 0) || (rmm->length == 0) || (rmm->height == 0)) {
		
		gchar         *buffer;
				
		g_io_channel_read_line (rmm->channel_output, &buffer, NULL, NULL, NULL);
				
		if (buffer == NULL)
			continue;

		if (g_str_has_prefix (buffer, "ID_VIDEO_WIDTH="))
			sscanf (buffer+15, "%i", &(rmm->width));
		
		if (g_str_has_prefix (buffer, "ID_VIDEO_HEIGHT="))
			sscanf (buffer+16, "%i", &(rmm->height));

		if (g_str_has_prefix (buffer, "ID_LENGTH="))
			sscanf (buffer+10, "%i", &(rmm->length));
				
		g_free (buffer);
	}
	
	rmm->org_width = rmm->width;
	rmm->org_height = rmm->height;
	
	gtk_widget_set_size_request (GTK_WIDGET (rmm), rmm->width, rmm->height);
	rmm->watch_out_id = g_io_add_watch (rmm->channel_output, G_IO_IN, rf_media_mplayer_output_lookup, rmm);
	g_timeout_add (100, rf_media_mplayer_timeout, rmm);
	
	return 0;
}

void
rf_media_mplayer_stop (GtkWidget *widget) {
	
	RfMediaMplayer  *rmm;
	
	g_return_if_fail (widget != NULL);
	g_return_if_fail (IS_RF_MEDIA_MPLAYER (widget));

	rmm = RF_MEDIA_MPLAYER (widget);
	
	if (rmm->mp_pid >0) {
		GSource *watch_source = g_main_context_find_source_by_id (NULL, rmm->watch_out_id);
		
		g_source_destroy (watch_source);
		g_io_channel_shutdown (rmm->channel_input, FALSE, NULL);
		g_io_channel_shutdown (rmm->channel_output, FALSE, NULL);
		rmm->channel_input = NULL;

		fclose (rmm->stream_input);
		
		kill (rmm->mp_pid, SIGKILL);
		kill (rmm->mp_pid, SIGKILL);
		rmm->mp_pid = -1;
		rmm->ready = TRUE;
	}
}

void
rf_media_mplayer_restart (GtkWidget *widget) {
	
	RfMediaMplayer  *rmm;
	
	g_return_if_fail (widget != NULL);
	g_return_if_fail (IS_RF_MEDIA_MPLAYER (widget));

	rmm = RF_MEDIA_MPLAYER (widget);

	if (rf_media_mplayer_is_running (widget))
		rf_media_mplayer_stop (widget);

	if (rmm->ready)
		rf_media_mplayer_launch (widget);
}

gpointer
rf_media_mplayer_event_thread (gpointer data) {

	RfMediaMplayer    *rmm = RF_MEDIA_MPLAYER (data);
	GtkWidget         *widget = GTK_WIDGET (data);
	XEvent             report;
	
	while (1) {
		if (XCheckWindowEvent (GDK_WINDOW_XDISPLAY (widget->window), GDK_WINDOW_XID (widget->window), KeyPressMask, &report)) {
			
			//report.xany.window = GDK_WINDOW_XID (widget->window);
			//report.xkey.window = GDK_WINDOW_XID (widget->window);
			
			//XSendEvent (GDK_WINDOW_XDISPLAY (widget->window), GDK_WINDOW_XID (widget->window), False, SubstructureRedirectMask, &report);
			//g_printf ("\t>> wcisnieto klawisz <<\n");

		}
		/*GdkEvent *event = gdk_event_get ();

		if (event != NULL)
		switch (event->type) {
			case GDK_KEY_PRESS:
			case GDK_KEY_RELEASE:
				g_printf ("key press/release\n");
				gdk_event_free (event);
				break;
		}*/
	}
}

void
rf_media_mplayer_open (GtkWidget *widget, gchar *file) {
	
	gint            pid;

	g_return_if_fail (widget != NULL);
	g_return_if_fail (IS_RF_MEDIA_MPLAYER (widget));

	RfMediaMplayer *rmm = RF_MEDIA_MPLAYER (widget);
	
	rmm->file = g_strdup (file);
	pid = rf_media_mplayer_launch (GTK_WIDGET (widget));

	GThread *mplayer_thread = NULL;

	mplayer_thread = g_thread_create (rf_media_mplayer_event_thread, rmm, FALSE, NULL);
}

gboolean
rf_media_mplayer_output_lookup (GIOChannel *source, GIOCondition condition, gpointer data) {

	RfMediaMplayer  *rmm;
	FILE            *stream;
	gint             i = 0;
	gchar            buffer[1024];
	
	g_return_if_fail (data != NULL);
	g_return_if_fail (IS_RF_MEDIA_MPLAYER (data));
	
	rmm = RF_MEDIA_MPLAYER (data);
	
	stream = fdopen (rmm->mp_out, "r");
	fflush (stream);
	
	do {
		
		buffer[i] = fgetc (stream);
		i++;

	} while ((buffer[i-1] != 0x0D) && (buffer[i-1] != '\n') && (buffer[i-1] != '\0' && (i < 1023)));

	buffer[i-1] = '\0';

	if (buffer != NULL)
	if (g_str_has_prefix (buffer, "A:"))
		sscanf (buffer + 2, "%d.", &(rmm->timer));
	
	return TRUE;
}

gboolean
rf_media_mplayer_timeout (gpointer data) {
	
	RfMediaMplayer  *rmm;
	gchar           *path;
	FILE            *fp;
	gint             i = 0;
	
	g_return_if_fail (data != NULL);
	g_return_if_fail (IS_RF_MEDIA_MPLAYER (data));

	rmm = RF_MEDIA_MPLAYER (data);
	
	path = g_strdup_printf("/proc/%d", rmm->mp_pid);
	
	fp = (FILE *) g_fopen (path, "r");
	
	if (fp == NULL) {
		
		//rf_media_mplayer_stop (GTK_WIDGET (rmm));
		rf_media_mplayer_restart (GTK_WIDGET (rmm));
		
		return FALSE;
		
	} else {	
		fclose (fp);
		rmm->ready = FALSE;
	}
	
	g_free (path);

	return TRUE;
}

static void
rf_media_mplayer_size_allocate (GtkWidget *widget, GtkAllocation *allocation) {

	RfMediaMplayer *rmm;
	
	g_return_if_fail (widget != NULL);
	g_return_if_fail (IS_RF_MEDIA_MPLAYER (widget));
	
	rmm = RF_MEDIA_MPLAYER (widget);
	
		
	rmm->width  = allocation->width;
	rmm->height = allocation->height;

	if (GTK_WIDGET_REALIZED (widget))
		gdk_window_move_resize (widget->window, allocation->x, allocation->y, rmm->width, rmm->height);
	
	if (rmm->ready) {
		rf_media_mplayer_restart (widget);
		gdk_window_move_resize (widget->window, allocation->x, allocation->y, 320, 200);
	}
	
	widget->allocation = *allocation;

}

static gboolean
rf_media_mplayer_expose (GtkWidget *widget, GdkEventExpose *event, gpointer user_data) {
	
	RfMediaMplayer *rmm;

	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (IS_RF_MEDIA_MPLAYER (widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);
	
	rmm = RF_MEDIA_MPLAYER (widget);
	
	return (TRUE);
	
}

static void
rf_media_mplayer_init (RfMediaMplayer *rmm) {
	
}

static void
rf_media_mplayer_realize (GtkWidget *widget) {
	
	RfMediaMplayer      *this;
	GdkWindowAttr        attributes;
	
	g_return_if_fail (widget != NULL);
	g_return_if_fail (IS_RF_MEDIA_MPLAYER (widget));

	this = RF_MEDIA_MPLAYER (widget);

	attributes.x                 = 0;
	attributes.y                 = 0;
	attributes.width             = 320;
	attributes.height            = 200;
	attributes.window_type       = GDK_WINDOW_CHILD;
	attributes.wclass            = GDK_INPUT_OUTPUT;
	attributes.visual            = gtk_widget_get_default_visual ();
	attributes.colormap          = gtk_widget_get_default_colormap ();
	attributes.event_mask        = GDK_ALL_EVENTS_MASK;
	widget->window               = gdk_window_new (widget->parent->window, &attributes, GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP);
	this->mp_pid                 = -1;
	
	gdk_window_set_user_data (widget->window, widget);

	GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
	gtk_widget_set_size_request (GTK_WIDGET (this), attributes.width, attributes.height);

}

static void
rf_media_mplayer_class_init (RfMediaMplayerClass *class) {
	
	GtkObjectClass *object_class;
	GtkWidgetClass *widget_class;
	
	object_class = (GtkObjectClass*) class;
	widget_class = (GtkWidgetClass*) class;
	
	parent_class = gtk_type_class (gtk_widget_get_type ());
	
	widget_class->realize               = (void *) rf_media_mplayer_realize;
	widget_class->size_allocate         = (void *) rf_media_mplayer_size_allocate;
	//widget_class->expose_event          = (void *) rf_media_mplayer_expose;

}

GType
rf_media_mplayer_get_type (void) {
	
	static GType rmm_type = 0;
	
	if (!rmm_type) {
	
		static const GTypeInfo rmm_info = {
			sizeof (RfMediaMplayerClass),
			NULL, /* base_init */
			NULL, /* base_finalize */
			(GClassInitFunc) rf_media_mplayer_class_init,
			NULL, /* class_finalize */
			NULL, /* class_data */
			sizeof (RfMediaMplayer),
			0,    /* n_preallocs */
			(GInstanceInitFunc) rf_media_mplayer_init,
		};
		
		rmm_type = g_type_register_static (GTK_TYPE_WIDGET, "RfMediaMplayer", &rmm_info, 0);
	}
	
	return rmm_type;

}

GtkWidget *
rf_media_mplayer_new (void) {
	
	return GTK_WIDGET (g_object_new (rf_media_mplayer_get_type (), NULL));
	
}
