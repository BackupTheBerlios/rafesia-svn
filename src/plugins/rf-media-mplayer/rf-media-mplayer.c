#include <gtk/gtk.h>
#include "rf-media-mplayer.h"

#include <stdlib.h>
#include <gdk/gdkx.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>

static GtkWidgetClass *parent_class = NULL;

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
rf_media_mplayer_launch (GtkWidget *widget, const gchar *command) {
	
	gint                pid, status;
	extern char       **environ;
	RfMediaMplayer     *player = RF_MEDIA_MPLAYER (widget);
	
	if (command == 0)
		return 1;
	
	pid = fork();
	if (pid == -1)
		return -1;
	
	if (pid == 0) {
		
		gchar *argv[4];
		
		argv[0] = "sh";
		argv[1] = "-c";
		argv[2] = g_strdup (command);
		argv[3] = 0;
		
		execve ("/bin/sh", argv, environ);
		exit (127);
		
		g_free (argv[0]);
		g_free (argv[1]);
		g_free (argv[2]);
		g_free (argv[3]);
		
	}
	
	do {
		if (waitpid (pid, &status, 0) == -1) {
			
			if (errno != EINTR)
				return -1;
			
		} else {
			
			player->mp_pid=pid+1; 
			return status;
			
		}

	} while(1);

}

void
rf_media_mplayer_stop (GtkWidget *widget) {
	
	RfMediaMplayer  *rmm;
	
	g_return_if_fail (widget != NULL);
	g_return_if_fail (IS_RF_MEDIA_MPLAYER (widget));

	rmm = RF_MEDIA_MPLAYER (widget);
	
	if (rmm->mp_pid >0) {
		kill (rmm->mp_pid, SIGKILL);
		kill (rmm->mp_pid, SIGKILL);
		rmm->mp_pid = -1;
	}
}

void
rf_media_mplayer_restart (GtkWidget *widget) {
	
	RfMediaMplayer  *rmm;
	
	g_return_if_fail (widget != NULL);
	g_return_if_fail (IS_RF_MEDIA_MPLAYER (widget));

	rmm = RF_MEDIA_MPLAYER (widget);

	/*if (rmm->input_file == NULL)
		rmm->input_file = g_strdup ("~/.rafesia_in");

	if (rmm->input == NULL)
		rmm->input = g_io_channel_new_file (rmm->input_file, "w", NULL);
	
	if (rmm->output_file == NULL)
		rmm->output_file = g_strdup ("~/.rafesia_out");

	if (rmm->output == NULL) 
		rmm->output = g_io_channel_new_file (rmm->output_file, "r", NULL);*/

	if (rf_media_mplayer_is_running (widget)) {
		
		rf_media_mplayer_stop (widget);
		rmm->ready = TRUE;

	}

	if (rmm->ready) {
		
		gchar *cmd;
		
		if (rmm->timer)
			rmm->timer = 0;

		//cmd = g_strdup_printf ("%s > mplayer -vo xv -slave -vop scale=%d:-3 -wid %d %s > %s 2> /dev/null &", rmm->input_file, rmm->width, GDK_WINDOW_XWINDOW (widget->window),rmm->file, rmm->output_file);
		cmd = g_strdup_printf ("mplayer -vo xv -slave -vop scale=%d:-3 -wid %d %s > %s 2> /dev/null &", rmm->width, GDK_WINDOW_XWINDOW (widget->window),rmm->file, rmm->output_file);
		rmm->ready = FALSE;
		rmm->timer = 0;
		g_timeout_add (100, rf_media_mplayer_timeout, rmm);
		rf_media_mplayer_launch (widget, cmd);
		g_free (cmd);
		
	}

}

void
rf_media_mplayer_play (GtkWidget *widget, gchar *file) {
	
	gchar          *cmd;
	gint            pid;
	GError         *error;

	g_return_if_fail (widget != NULL);
	g_return_if_fail (IS_RF_MEDIA_MPLAYER (widget));

	RfMediaMplayer *rmm = RF_MEDIA_MPLAYER (widget);
	
	/*if (rmm->input_file == NULL)
		rmm->input_file = g_strdup ("~/.rafesia_in");

	if (rmm->input == NULL) {
		rmm->input = g_io_channel_new_file (rmm->input_file, "rwa", &error);
		g_io_channel_flush (rmm->input, NULL);
	}
	
	if (rmm->output_file == NULL)
		rmm->output_file = g_strdup ("~/.rafesia_out");

	if (rmm->output == NULL)  {
		rmm->output = g_io_channel_new_file (rmm->output_file, "rwa", NULL);
		g_io_channel_flush (rmm->output, NULL);
	}*/
	
	rmm->file = g_strdup (file);
	//cmd = g_strdup_printf ("%s > mplayer -vo xv -slave -vop scale=%d:-3 -wid %d %s > %s 2> /dev/null &", rmm->input_file, rmm->width, GDK_WINDOW_XWINDOW (widget->window),rmm->file, rmm->output_file);
	cmd = g_strdup_printf ("mplayer -vo xv -slave -vop scale=%d:-3 -wid %d %s > %s 2> /dev/null &", rmm->width, GDK_WINDOW_XWINDOW (widget->window),rmm->file, rmm->output_file);
	pid = rf_media_mplayer_launch (GTK_WIDGET (widget), cmd);
	rmm->timer = 0;
	rmm->ready = FALSE;
	g_timeout_add (100, rf_media_mplayer_timeout, rmm);
	g_free (cmd);
	
}

gboolean
rf_media_mplayer_timeout (gpointer data) {
	
	RfMediaMplayer  *rmm;
	gchar           *path;
	FILE            *fp;
	
	g_return_if_fail (data != NULL);
	g_return_if_fail (IS_RF_MEDIA_MPLAYER (data));

	rmm = RF_MEDIA_MPLAYER (data);
	
	path = g_strdup_printf("/proc/%d", rmm->mp_pid);
	
	fp = (FILE *) g_fopen (path, "r");
	if (fp == NULL) {
		rf_media_mplayer_restart (GTK_WIDGET (rmm));
		return FALSE;
	} else
		fclose (fp);
	
	g_free (path);
	rmm->timer++;
	
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
		gdk_window_move_resize (widget->window, allocation->x, allocation->y, allocation->width, rmm->height);
	
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
	
	this->input                  = NULL;
	this->input_file             = g_strdup ("~/.rafesia_in");
	this->output                 = NULL;
	this->output_file            = g_strdup ("~/.rafesia_out");
	
	gdk_window_set_user_data (widget->window, widget);

	GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
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
	widget_class->expose_event          = (void *) rf_media_mplayer_expose;

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
