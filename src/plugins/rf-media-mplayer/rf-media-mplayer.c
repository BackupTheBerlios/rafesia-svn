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
	RfMediaMplayer     *rmm = RF_MEDIA_MPLAYER (widget);
	
	if (pipe (rmm->pipe_input) != 0)
		g_printf ("input pipe error\n");

	if (pipe (rmm->pipe_output) != 0)
		g_printf ("output pipe error\n");
	
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
		
		close (rmm->pipe_input[1]);
		close (rmm->pipe_output[0]);
		if (dup2 (rmm->pipe_input[0], 0) != 0)
			g_printf ("input dup error\n");
		
		//if (dup2 (rmm->pipe_output[1], 1) != 0)
		//	g_printf ("output dup error\n");
		
		execve ("/bin/sh", argv, environ);
		exit (127);
		
		g_free (argv[0]);
		g_free (argv[1]);
		g_free (argv[2]);
		g_free (argv[3]);
		
	}
	
	/*
	FILE *fp;
	gchar buf [1024];
	fp = fdopen (rmm->pipe_output[0], "r");
	while (!feof (fp) && !ferror (fp) && fgets (buf, sizeof (buf), fp) != NULL)
		fputs (buf, stdout);
	*/
	
	do {
		if (waitpid (pid, &status, 0) == -1) {
			
			if (errno != EINTR)
				return -1;
			
		} else {
			
			rmm->mp_pid=pid+1; 
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

	if (rf_media_mplayer_is_running (widget)) {
		
		rf_media_mplayer_stop (widget);
		rmm->ready = TRUE;

	}

	if (rmm->ready) {
		
		gchar *cmd;
		
		if (rmm->timer)
			rmm->timer = 0;

		cmd = g_strdup_printf ("mplayer -identify -vo xv -slave -osdlevel 0 -nolirc -noautosub -nograbpointer -nokeepaspect -nomouseinput -noconsolecontrols -vop scale=%d:-3 -wid %d %s 2> /dev/null &", rmm->width, GDK_WINDOW_XWINDOW (widget->window),rmm->file);
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
	
	rmm->file = g_strdup (file);
	// -ss <sek>   <- odtwarza od tego miejsca
	cmd = g_strdup_printf ("mplayer -identify -vo xv -slave -osdlevel 0 -nolirc -noautosub -nograbpointer -nokeepaspect -nomouseinput -noconsolecontrols -vop scale=%d:-3 -wid %d %s 2> /dev/null &", rmm->width, GDK_WINDOW_XWINDOW (widget->window),rmm->file);
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
	
	gdk_window_set_user_data (widget->window, widget);

	GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
	gtk_widget_set_usize (GTK_WIDGET (this), 320, 240);

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
