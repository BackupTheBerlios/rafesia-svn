#include <xine.h>
#include "rf-media-xine.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>
#include <X11/extensions/XTest.h>

static GtkWidgetClass     *parent_class = NULL;

static void* xine_thread (void *this_gen) {
	RfMediaXine    *this   = (RfMediaXine *) this_gen;


	while (1) {


	XEvent event;
	XNextEvent (gdk_x11_get_default_xdisplay(), &event);

	//GdkEvent *event;
	//event =  gdk_get_event();
	//gdk_event_get_graphics_expose(this->video_window);
	
	switch (event.type) {
		case Expose:
			if (event.xexpose.count != 0)
				break;
			xine_gui_send_vo_data (this->stream, XINE_GUI_SEND_EXPOSE_EVENT, &event);
			break;
	}

	//if (event.type == this->completion_event)
	//	xine_gui_send_vo_data (this->stream, XINE_GUI_SEND_COMPLETION_EVENT, &event);

	}

	return NULL;
}

/*gpointer
xine_thread (gpointer data) {

	RfMediaXine      *rmx = (RfMediaXine *) data;
	GdkEvent         *event;

	while (1) {
	
		event = gdk_event_get();
		
		if (event != NULL) {
			g_printf("event: %d\n", event->type);
		
			switch (event->type) {
				case GDK_EXPOSE:
					g_printf("jestem... haha\n");
					//xine_gui_send_gui_data (rmx->stream, XINE_GUI_SEND_EXPOSE_EVENT, &event);
					break;
			}
		//	xine_gui_send_vo_data (rmx->stream, XINE_GUI_SEND_EXPOSE_EVENT, &event);

			gdk_event_free(event);
		}
	}
	
	return NULL;
	
}*/

static gint 
rf_media_xine_expose (GtkWidget *widget, GdkEventExpose *event, gpointer user_data) {
	
	RfMediaXine       *rmx = (RfMediaXine *) user_data;
	
	xine_gui_send_vo_data (rmx->stream, XINE_GUI_SEND_EXPOSE_EVENT, event);
	
	return (TRUE);
	
}

void
rf_media_xine_instance_init (GTypeInstance *instance, gpointer g_class) {
	
	RfMediaXine *this = (RfMediaXine *)instance;
	
	this->widget.requisition.width  = 320;
	this->widget.requisition.height = 200;

	this->xine                      = xine_new();
	snprintf (this->configfile, 255, "%s/.gxine/config", getenv ("HOME"));
	xine_config_load (this->xine, this->configfile);

	xine_engine_set_param (this->xine, XINE_ENGINE_PARAM_VERBOSITY, 0);

	this->stream                    = NULL;
	this->vo_port                   = NULL;
	this->ao_port                   = NULL;

xine_init (this->xine);	
	
}

static void 
dest_size_cb(void *data, int video_width, int video_height, double video_pixel_aspect, int *dest_width, int *dest_height, double *dest_pixel_aspect) {
		
	RfMediaXine        *rmx = (RfMediaXine *)data;
	
	*dest_width        = 320;
	*dest_height       = 200;
	*dest_pixel_aspect = gdk_screen_width()/gdk_screen_height();

}

static void 
frame_output_cb(void *data, int video_width, int video_height, double video_pixel_aspect, int *dest_x, int *dest_y, int *dest_width, int *dest_height, double *dest_pixel_aspect, int *win_x, int *win_y) {
	
	RfMediaXine        *rmx = (RfMediaXine *)data;
	
	if (rmx == NULL)
		return;

	gdk_window_get_geometry (rmx->video_window, win_x, win_y, dest_width, dest_height, NULL);
	*dest_x            = 0;
	*dest_y            = 0;
	*dest_pixel_aspect = gdk_screen_width () / gdk_screen_height ();
	
}

static gboolean
configure_cb (GtkWidget *widget, GdkEventConfigure *event, RfMediaXine *rmx) {

	//rmx->priv->xpos = event->x + GTK_WIDGET (rmx)->allocation.x;
	//mx->priv->ypos = event->y + GTK_WIDGET (rmx)->allocation.y;
	
	gdk_window_resize(rmx->video_window, event->x, event->y);
	g_printf("conf\n");
	return FALSE;
	
}

/*static void
size_changed_cb (GdkScreen *screen, BaconVideoWidget *bvw)
{
	double res_h, res_v;

	XLockDisplay (bvw->priv->display);
	res_h = (DisplayWidth (bvw->priv->display, bvw->priv->screen) * 1000 /
			DisplayWidthMM (bvw->priv->display,
				bvw->priv->screen));
	res_v = (DisplayHeight (bvw->priv->display, bvw->priv->screen) * 1000 /
			DisplayHeightMM (bvw->priv->display,
				bvw->priv->screen));
	XUnlockDisplay (bvw->priv->display);

	bvw->priv->display_ratio = res_v / res_h;

	if (fabs (bvw->priv->display_ratio - 1.0) < 0.01) {
		bvw->priv->display_ratio = 1.0;
	}
}*/

static void 
rf_media_xine_realize (GtkWidget *widget) {

	RfMediaXine         *this;

	g_return_if_fail(widget != NULL);
	g_return_if_fail(IS_RF_MEDIA_XINE(widget));

	this = RF_MEDIA_XINE(widget);

	this->attributes.x           = 0;
	this->attributes.y           = 0;
	this->attributes.width       = 320;
	this->attributes.height      = 200;
	this->attributes.window_type = GDK_WINDOW_CHILD;
	this->attributes.wclass      = GDK_INPUT_OUTPUT;
	this->attributes.visual      = gtk_widget_get_default_visual ();
	this->attributes.colormap    = gtk_widget_get_default_colormap ();
	this->attributes.event_mask  = 0;
	this->video_window           = gdk_window_new(this->widget.parent->window, &(this->attributes), GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP);
	widget->window               = this->video_window;
	
	this->vis.display            = gdk_x11_get_default_xdisplay();
	this->vis.screen             = gdk_x11_get_default_screen();
	this->vis.d                  = GDK_WINDOW_XID(widget->window);
	this->vis.dest_size_cb       = dest_size_cb;
	this->vis.frame_output_cb    = frame_output_cb;
	this->vis.user_data          = widget;
	
	/*if (!XInitThreads ()) {
	
		g_printf ("rf-media-xine: XInitThreads failed - looks like you don't have a thread-safe xlib.\n");
		
		return ;
		
	}*/

	this->vo_port = xine_open_video_driver(this->xine, this->vo_driver, XINE_VISUAL_TYPE_X11, (void *)&(this->vis));
	this->ao_port = xine_open_audio_driver(this->xine , this->ao_driver, NULL);

	if (!this->vo_port) {
	
		g_printf ("rf-media-xine: couldn't open video driver\n");
		
		return ;
		
	}

	this->stream = xine_stream_new(this->xine, this->ao_port, this->vo_port);

	/* track configure events of toplevel window */
	//g_signal_connect (G_OBJECT (gtk_widget_get_toplevel (widget)), "configure-event", G_CALLBACK (configure_cb), this);
	g_signal_connect_after (G_OBJECT (gtk_widget_get_toplevel (widget)), "expose-event", G_CALLBACK (rf_media_xine_expose), this);
	//g_signal_connect (G_OBJECT (widget), "expose-event", G_CALLBACK (rf_media_xine_expose), this); 
	
	/* get screen size changes */
	//g_signal_connect (G_OBJECT (gdk_screen_get_default ()), "size-changed", G_CALLBACK (size_changed_cb), this);
	
	gtk_widget_set_events ( (GtkWidget *)this, GDK_EXPOSURE_MASK);
	GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);

	//g_thread_create (xine_thread, this, FALSE, NULL);
	return ;

}

static void 
rf_media_xine_size_allocate (GtkWidget *widget, GtkAllocation *allocation) {

	RfMediaXine *this;

	g_return_if_fail (widget != NULL);
	g_return_if_fail (IS_RF_MEDIA_XINE(widget));

	this = RF_MEDIA_XINE(widget);

	widget->allocation = *allocation;

	if (GTK_WIDGET_REALIZED (widget)) {
	
	  gdk_window_move_resize (widget->window,
	            allocation->x,
	            allocation->y,
	            allocation->width,
	            allocation->height);

	}
	
}

static void 
rf_media_xine_finalize (GObject *object) {

	RfMediaXine *rmx = (RfMediaXine *) object;

	G_OBJECT_CLASS (parent_class)->finalize (object);
	rmx = NULL;
	
}

void
rf_media_xine_class_init (gpointer class, gpointer class_data) {

	GObjectClass              *object_class;
	GtkWidgetClass            *widget_class;
	
	object_class = (GObjectClass *) class;
	widget_class = (GtkWidgetClass *) class;

	parent_class = gtk_type_class (gtk_widget_get_type ());

	/* GtkWidget */
	widget_class->realize                  = rf_media_xine_realize;
	//widget_class->unrealize                = rf_media_xine_unrealize;
	//widget_class->size_request             = rf_media_xine_size_request;
	widget_class->size_allocate            = rf_media_xine_size_allocate;
	widget_class->expose_event             = (void *)rf_media_xine_expose;
	//widget_class->motion_notify_event      = rf_media_xine_widget_motion_notify;
	//widget_class->button_press_event       = rf_media_xine_widget_button_press;

	/* GObject */
	object_class->set_property             = NULL;
	object_class->get_property             = NULL;
	object_class->finalize                 = rf_media_xine_finalize;

	/* Properties */
	/*g_object_class_install_property (object_class, PROP_LOGO_MODE,
			g_param_spec_boolean ("logo_mode", NULL, NULL,
				FALSE, G_PARAM_READWRITE));*/

	/* Signals */
	/*bvw_table_signals[ERROR] =
		g_signal_new ("error",
				G_TYPE_FROM_CLASS (object_class),
				G_SIGNAL_RUN_LAST,
				G_STRUCT_OFFSET (BaconVideoWidgetClass, error),
				NULL, NULL,
				baconvideowidget_marshal_VOID__STRING_BOOLEAN_BOOLEAN,
				G_TYPE_NONE, 3, G_TYPE_STRING, G_TYPE_BOOLEAN,
				G_TYPE_BOOLEAN);*/
				
}

GType 
rf_media_xine_get_type (void) {
	static GType xine_type = 0;
	
	if (!xine_type) {
		static const GTypeInfo xine_info = {
			sizeof (RfMediaXineClass),
			NULL, /* base_init */
			NULL, /* base_finalize */
			(GClassInitFunc) rf_media_xine_class_init,
			NULL, /* class_finalize */
			NULL, /* class_data */
			sizeof (RfMediaXine),
			0,
			(GInstanceInitFunc) rf_media_xine_instance_init,
		};
		
		xine_type = g_type_register_static (GTK_TYPE_WIDGET, "RF_XINE_MEDIA_TYPE", &xine_info, 0);
	}
	
	return (xine_type);
	
}

GtkWidget *
rf_media_xine_new (void) {
	
	GtkWidget *widget = GTK_WIDGET (g_object_new (rf_media_xine_get_type (), NULL));

	g_signal_connect (GTK_OBJECT (widget), "expose-event", G_CALLBACK (rf_media_xine_expose),NULL);
	
	return widget;
	
}
