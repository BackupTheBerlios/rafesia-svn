#include <xine.h>
#include "rf-media-xine.h"

static GtkWidgetClass     *parent_class = NULL;

static gboolean
rf_media_xine_expose (GtkWidget *widget, GdkEventExpose *event, gpointer user_data) {
	
	RfMediaXine *rmx = RF_MEDIA_XINE (widget);
	
	g_return_val_if_fail (widget != NULL, FALSE);
	g_return_val_if_fail (IS_RF_MEDIA_XINE (widget), FALSE);
	g_return_val_if_fail (event != NULL, FALSE);
	
	if (rmx->stream != NULL)
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
dest_size_cb (void *data, int video_width, int video_height, double video_pixel_aspect, int *dest_width, int *dest_height, double *dest_pixel_aspect) {
		
	RfMediaXine        *rmx = (RfMediaXine *)data;
	
	*dest_width        = 320;
	*dest_height       = 200;
	*dest_pixel_aspect = gdk_screen_width () / gdk_screen_height ();

}

static void 
frame_output_cb (void *data, int video_width, int video_height, double video_pixel_aspect, int *dest_x, int *dest_y, int *dest_width, int *dest_height, double *dest_pixel_aspect, int *win_x, int *win_y) {
	
	RfMediaXine        *rmx = (RfMediaXine *)data;
	
	if (rmx == NULL)
		return;

	gdk_window_get_geometry (rmx->video_window, win_x, win_y, dest_width, dest_height, NULL);
	*dest_x            = 0;
	*dest_y            = 0;
	*dest_pixel_aspect = gdk_screen_width () / gdk_screen_height ();
	
}

static void 
rf_media_xine_realize (GtkWidget *widget) {

	RfMediaXine         *this;
	GdkWindowAttr        attributes;
	
	g_return_if_fail (widget != NULL);
	g_return_if_fail (IS_RF_MEDIA_XINE(widget));

	this = RF_MEDIA_XINE (widget);

	attributes.x                 = 0;
	attributes.y                 = 0;
	attributes.width             = 320;
	attributes.height            = 200;
	attributes.window_type       = GDK_WINDOW_CHILD;
	attributes.wclass            = GDK_INPUT_OUTPUT;
	attributes.visual            = gtk_widget_get_default_visual ();
	attributes.colormap          = gtk_widget_get_default_colormap ();
	attributes.event_mask        = gtk_widget_get_events (widget) | GDK_EXPOSURE_MASK | GDK_POINTER_MOTION_MASK | GDK_KEY_PRESS_MASK;
	widget->window               = gdk_window_new (widget->parent->window, &attributes, GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP);
	this->video_window           = widget->window;
	
	this->vis.display            = gdk_x11_get_default_xdisplay();
	this->vis.screen             = gdk_x11_get_default_screen();
	this->vis.d                  = GDK_WINDOW_XID(widget->window);
	this->vis.dest_size_cb       = dest_size_cb;
	this->vis.frame_output_cb    = frame_output_cb;
	this->vis.user_data          = widget;
	
	this->vo_port = xine_open_video_driver (this->xine, this->vo_driver, XINE_VISUAL_TYPE_X11, (void *)&(this->vis));
	this->ao_port = xine_open_audio_driver (this->xine , this->ao_driver, NULL);

	if (!this->vo_port) {
		g_warning ("rf-media-xine: couldn't open video driver\n");
		return ;
	}

	this->stream = xine_stream_new (this->xine, this->ao_port, this->vo_port);

	gdk_window_set_user_data (widget->window, widget);
//	gtk_style_set_background (widget->style, widget->window, GTK_STATE_ACTIVE);
	GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
	
	return;

}

static void 
rf_media_xine_size_allocate (GtkWidget *widget, GtkAllocation *allocation) {

	RfMediaXine *this;

	g_return_if_fail (widget != NULL);
	g_return_if_fail (IS_RF_MEDIA_XINE(widget));

	this = RF_MEDIA_XINE(widget);

	widget->allocation = *allocation;

	if (GTK_WIDGET_REALIZED (widget))
		gdk_window_move_resize (widget->window, allocation->x, allocation->y, allocation->width, allocation->height);
	
}

static void 
rf_media_xine_finalize (GObject *object) {

	G_OBJECT_CLASS (parent_class)->finalize (object);
	
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

	/* GObject */
	object_class->set_property             = NULL;
	object_class->get_property             = NULL;
	object_class->finalize                 = rf_media_xine_finalize;

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

gboolean
rf_media_xine_key_press ( GtkWidget *this,  GdkEventKey *event, gpointer user_data) {
	
	gtk_signal_emit_by_name (GTK_OBJECT (gtk_widget_get_toplevel (GTK_WIDGET (this))),  "key_press_event", event, NULL);
	
}

GtkWidget *
rf_media_xine_new (void) {
	
	GtkWidget *widget = GTK_WIDGET (g_object_new (rf_media_xine_get_type (), NULL));
	
	return widget;
	
}
