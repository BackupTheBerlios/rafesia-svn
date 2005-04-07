#include "rf-widget-mcontainer.h"
#include "../plugins/plugins.h"
#include "rf-window-main.h"

static GtkWidgetClass     *parent_class = NULL;

static void
rf_mcontainer_add (GtkContainer *container, GtkWidget *widget) {
	
	RfMContainer *rcont;
	
	g_return_if_fail (IS_RF_MCONTAINER (container));
	g_return_if_fail (widget != NULL);
	
	rcont = RF_MCONTAINER (container);
	rcont->child = widget;
	
	gtk_widget_set_parent (widget, GTK_WIDGET (container));
	
}

static void
rf_mcontainer_size_allocate (GtkWidget *widget, GtkAllocation *allocation) {
	
	RfMContainer  *rcont;
	MediaModule   *mmod = get_mediamodule ();
	gint           movie_width, movie_height, width, height;
	gfloat         mx, my, x, y;
	gfloat         ratio, nratio;
	gchar         *res;
	
	g_return_if_fail (widget != NULL);
	g_return_if_fail (IS_RF_MCONTAINER (widget));
	
	rcont = RF_MCONTAINER (widget);
	
	if (GTK_WIDGET_REALIZED (widget))
		gdk_window_move_resize (widget->window, allocation->x, allocation->y, allocation->width, allocation->height);
	
	width = allocation->width;
	height = allocation->height;
	mmod->get_movie_size (mmod, &movie_height, &movie_width);
	
	if (rcont->child == NULL || movie_width == 0 || movie_height == 0)
		return;
	
	/*
	 * FIXME: poprawic koniecznie obliczanie ratio !!!
	 */
	
	res = g_strdup_printf ("%dx%d", movie_width, movie_height);
	sscanf (res, "%fx%f", &mx, &my);
	ratio = mx / my;
	g_free (res);

	res = g_strdup_printf ("%dx%d", width, height);
	sscanf (res, "%fx%f", &x, &y);
	nratio = x / y;
	g_free (res);
	
	if (ratio > nratio) {
		
		// obszar za wysoki, margines w pionie
	
		gfloat           dx, tmp;
		gint             nhig;
		GtkAllocation    ch_alloc;
		
		dx = x / mx;
		tmp = dx * my;
		res = g_strdup_printf ("%f", tmp);
		sscanf (res, "%d,", &nhig);
		g_free (res);
		
		ch_alloc.x = 0;
		ch_alloc.y = (height - nhig) / 2;
		ch_alloc.width = width;
		ch_alloc.height = nhig;
		
		gtk_widget_size_allocate (GTK_WIDGET (rcont->child), &ch_alloc);
		
	} else {
		
		// Obszar za szeroki, margines w poziomie
		
		gfloat           dy, tmp;
		gint             nwid;
		GtkAllocation    ch_alloc;

		dy = y / my;
		tmp = dy * mx;
		res = g_strdup_printf ("%f", tmp);
		sscanf (res, "%d,", &nwid);
		g_free (res);
		
		ch_alloc.x = (width - nwid) / 2;
		ch_alloc.y = 0;
		ch_alloc.width = nwid;
		ch_alloc.height = height;
		
		gtk_widget_size_allocate (GTK_WIDGET (rcont->child), &ch_alloc);
	}
	
	widget->allocation = *allocation;
}

static void 
rf_mcontainer_finalize (GObject *object) {

	G_OBJECT_CLASS (parent_class)->finalize (object);
	
}

static void
rf_mcontainer_realize (GtkWidget *widget) {
	
	GdkWindowAttr  attributes;
	GdkColor       color;
	RfMContainer  *rcont;
	
	g_return_if_fail (IS_RF_MCONTAINER (widget));
	rcont = RF_MCONTAINER (widget);
	
	GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);
	
	attributes.window_type = GDK_WINDOW_CHILD;
	attributes.x = widget->allocation.x;
	attributes.y = widget->allocation.y;
	attributes.width = widget->allocation.width;
	attributes.height = widget->allocation.height;
	attributes.wclass = GDK_INPUT_OUTPUT;
	attributes.visual = gtk_widget_get_visual (widget);
	attributes.colormap = gtk_widget_get_colormap (widget);
	
	widget->window = gdk_window_new (gtk_widget_get_parent_window (widget), &attributes, GDK_VISIBILITY_NOTIFY_MASK);
	gdk_window_set_user_data (widget->window, widget);
	
	attributes.event_mask = GDK_EXPOSURE_MASK | GDK_SCROLL_MASK | gtk_widget_get_events (widget);
	
	widget->style = gtk_style_attach (widget->style, widget->window);
	gdk_color_parse ("black", &color);
	gtk_widget_modify_bg (widget, GTK_STATE_NORMAL, &color);

}

static void
rf_mcontainer_init (RfMContainer *rcont) {
	
	GtkWidget *cont = GTK_WIDGET (rcont);

	rcont->child = NULL;
}

void
rf_mcontainer_class_init (gpointer class, gpointer class_data) {

	GObjectClass              *object_class;
	GtkContainerClass         *container_class;
	GtkWidgetClass            *widget_class;
	
	object_class = (GObjectClass *) class;
	container_class = (GtkContainerClass *) class;
	widget_class = (GtkWidgetClass *) class;
	
	parent_class = gtk_type_class (gtk_container_get_type ());
	
	/* GtkContainer */
	container_class->add                   = rf_mcontainer_add;
	
	/* GtkWidget */
	widget_class->realize                  = rf_mcontainer_realize;
	widget_class->size_allocate            = rf_mcontainer_size_allocate;
		
	/* GObject */
	object_class->set_property             = NULL;
	object_class->get_property             = NULL;
	object_class->finalize                 = rf_mcontainer_finalize;
	
}

GType 
rf_mcontainer_get_type (void) {
	static GType type = 0;
	
	if (!type) {
		static const GTypeInfo info = {
			sizeof (RfMContainerClass),
			NULL, /* base_init */
			NULL, /* base_finalize */
			(GClassInitFunc) rf_mcontainer_class_init,
			NULL, /* class_finalize */
			NULL, /* class_data */
			sizeof (RfMContainer),
			0,
			(GInstanceInitFunc) rf_mcontainer_init,
		};
		
		type = g_type_register_static (GTK_TYPE_CONTAINER, "RF_MCONTAINER_TYPE", &info, 0);
	}
	
	return (type);
	
}

GtkWidget *
rf_mcontainer_new (void) {
	
	return GTK_WIDGET (g_object_new (rf_mcontainer_get_type (), NULL));
	
}
