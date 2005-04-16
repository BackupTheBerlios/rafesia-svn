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
	gchar              *argv[] = {"mplayer", "-wid", g_strdup_printf ("%d", GDK_WINDOW_XWINDOW (rmm->mp_window)), "-identify", "-vo", "xv", "-slave", "-osdlevel", "0", "-nolirc", rmm->file, '\0'};
//	gchar              *argv[] = {"mplayer", "-wid", g_strdup_printf ("%d", GDK_WINDOW_XWINDOW (rmm->mp_window)), "-identify", "-vo", "xv", "-slave", "-osdlevel", "0", "-nolirc", "-noautosub", rmm->file, '\0'};

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

void
rf_media_mplayer_open (GtkWidget *widget, gchar *file) {
	
	gint            pid;
	RfMediaMplayer *rmm;
	
	g_return_if_fail (widget != NULL);
	g_return_if_fail (IS_RF_MEDIA_MPLAYER (widget));
	
	rmm = RF_MEDIA_MPLAYER (widget);
	
	rmm->file = g_strdup (file);
	pid = rf_media_mplayer_launch (GTK_WIDGET (widget));
	
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
	
	if (GTK_WIDGET_REALIZED (widget)) {
		
		gfloat   x, y, mx, my;
		gfloat   ratio, nratio;
		gchar   *tmp;
		
		gdk_window_move_resize (widget->window, allocation->x, allocation->y, rmm->width, rmm->height);
		
		/*
		 * FIXME: poprawic koniecznie obliczanie ratio !!!
		 */
	
		tmp = g_strdup_printf ("%dx%d", rmm->org_width, rmm->org_height);
		sscanf (tmp, "%fx%f", &mx, &my);
		ratio = mx / my;
		g_free (tmp);

		tmp = g_strdup_printf ("%dx%d", rmm->width, rmm->height);
		sscanf (tmp, "%fx%f", &x, &y);
		nratio = x / y;
		g_free (tmp);
	
		if (ratio > nratio) {
			
			// obszar za wysoki, margines w pionie
			
			gfloat           dx, tmp2;
			gint             nhig;
			GtkAllocation    ch_alloc;
			
			dx = x / mx;
			tmp2 = dx * my;
			tmp = g_strdup_printf ("%f", tmp2);
			sscanf (tmp, "%d,", &nhig);
			g_free (tmp);
			
			gdk_window_move_resize (rmm->mp_window, 0, (rmm->height - nhig) / 2, rmm->width, nhig);
			
		} else {
			
			// Obszar za szeroki, margines w poziomie
			
			gfloat           dy, tmp2;
			gint             nwid;
			GtkAllocation    ch_alloc;
			
			dy = y / my;
			tmp2 = dy * mx;
			tmp = g_strdup_printf ("%f", tmp2);
			sscanf (tmp, "%d,", &nwid);
			g_free (tmp);
			
			gdk_window_move_resize (rmm->mp_window, (rmm->width - nwid) / 2, 0, nwid, rmm->height);
			
		}
		
	}
	
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

GdkFilterReturn
mp_window_filter (GdkXEvent *xevent, GdkEvent *event, gpointer user_data) {
	
	RfMediaMplayer      *rmm;
	GtkWidget           *widget;
	XEvent              *xev;
	gint                 result = GDK_FILTER_REMOVE;
	
	g_return_if_fail (xevent != NULL);
	g_return_if_fail (event != NULL);
	g_return_if_fail (user_data != NULL);
	
	rmm = RF_MEDIA_MPLAYER (user_data);
	widget = GTK_WIDGET (user_data);
	
	xev = (XEvent *) xevent;
	xev->xany.window = GDK_WINDOW_XWINDOW (widget->parent->window);
	
	switch (xev->type) {
		case KeyPress:
		case KeyRelease:
			xev->xkey.window = GDK_WINDOW_XWINDOW (widget->window);
			break;
			
		case ButtonPress:
		case ButtonRelease:
			xev->xbutton.window  = GDK_WINDOW_XWINDOW (widget->window);
			break;
			
		case MotionNotify:
		case EnterNotify:
		case LeaveNotify:
			xev->xmotion.window = GDK_WINDOW_XWINDOW (widget->window);
			break;

		case FocusIn:
		case FocusOut:
		case KeymapNotify:
		case Expose:
		case GraphicsExpose:
		case NoExpose:
		case VisibilityNotify:
		case CreateNotify:
		case DestroyNotify:
		case UnmapNotify:
		case MapNotify:
		case MapRequest:
		case ReparentNotify:
		case ConfigureNotify:
		case ConfigureRequest:
		case GravityNotify:
		case ResizeRequest:
		case CirculateNotify:
		case CirculateRequest:
		case PropertyNotify:
		case SelectionClear:
		case SelectionRequest:
		case SelectionNotify:
		case ColormapNotify:
		case ClientMessage:
		case MappingNotify:
			return GDK_FILTER_REMOVE;
	}
	
	return result;
	
}

void
mp_cb (GdkEvent *event, gpointer data) {
	
	RfMediaMplayer *rmm = RF_MEDIA_MPLAYER (data);
	GtkWidget *widget = GTK_WIDGET (data);
	
	//if (event->any.window == rmm->mp_window)
	//	event->any.window = rmm->mp_window;
	
	switch (event->type) {
		
		/*case GDK_MOTION_NOTIFY:
			event->any.window = widget->window;
			event->motion.window = widget->window;
			break;
		*/	
		/*
		case GDK_BUTTON_PRESS:
		case GDK_2BUTTON_PRESS:
		case GDK_3BUTTON_PRESS:
		case GDK_BUTTON_RELEASE:
			g_printf ("button\n");
			event->button.window = widget->window;
			break;
		*/
		
		case GDK_KEY_PRESS:
		case GDK_KEY_RELEASE:
			g_printf ("key\n");
			return;
			if (event->key.window == rmm->mp_window) {
				
				event->any.window = widget->parent->window;
				event->key.window = widget->parent->window;
				
			}
			break;
			
		//case GDK_ENTER_NOTIFY:
		//case GDK_LEAVE_NOTIFY:
			
		//case GDK_FOCUS_CHANGE:
		//	event->focus_change.window = widget->window;
		//	break;
			
		//case GDK_SCROLL:
		//	event->scroll.window = widget->window;
		//	break;
	}
	
	gtk_main_do_event (event);
	
}


static void
rf_media_mplayer_realize (GtkWidget *widget) {
	
	RfMediaMplayer      *this;
	GdkWindowAttr        attributes, mp_attrib;
	GdkColor             color;
	
	g_return_if_fail (widget != NULL);
	g_return_if_fail (IS_RF_MEDIA_MPLAYER (widget));
	
	this = RF_MEDIA_MPLAYER (widget);
	
	attributes.x                 = widget->allocation.x;
	attributes.y                 = widget->allocation.y;
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
	gdk_color_parse ("black", &color);
	gtk_widget_modify_bg (widget, GTK_STATE_NORMAL, &color);
	
	
	mp_attrib.window_type        = GDK_WINDOW_CHILD;
	mp_attrib.x                  = 0;
	mp_attrib.y                  = 0;
	mp_attrib.width              = widget->allocation.width;
	mp_attrib.height             = widget->allocation.height;
	mp_attrib.wclass             = GDK_INPUT_OUTPUT;
	mp_attrib.visual             = gtk_widget_get_visual (widget);
	mp_attrib.colormap           = gtk_widget_get_colormap (widget);
	mp_attrib.event_mask         = GDK_ALL_EVENTS_MASK;
	this->mp_window              = gdk_window_new (widget->window, &mp_attrib, GDK_VISIBILITY_NOTIFY_MASK);
	
	widget->style                = gtk_style_attach (widget->style, widget->window);
	
	//gdk_window_add_filter (GDK_WINDOW (this->mp_window), mp_window_filter, this);
	gdk_event_handler_set (mp_cb, this, NULL);
	gdk_window_set_user_data (this->mp_window, widget);
	
	GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
	gtk_widget_set_size_request (GTK_WIDGET (this), attributes.width, attributes.height);

}

static void
rf_media_mplayer_class_init (RfMediaMplayerClass *class) {
	
	GtkObjectClass *object_class;
	GtkWidgetClass *widget_class;
	
	object_class                        = (GtkObjectClass*) class;
	widget_class                        = (GtkWidgetClass*) class;
	
	parent_class                        = gtk_type_class (gtk_widget_get_type ());
	
	widget_class->realize               = rf_media_mplayer_realize;
	widget_class->size_allocate         = rf_media_mplayer_size_allocate;

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
			NULL,
		};
		
		rmm_type = g_type_register_static (GTK_TYPE_WIDGET, "RfMediaMplayer", &rmm_info, 0);
	}
	
	return rmm_type;

}

GtkWidget *
rf_media_mplayer_new (void) {
	
	return GTK_WIDGET (g_object_new (rf_media_mplayer_get_type (), NULL));
	
}
