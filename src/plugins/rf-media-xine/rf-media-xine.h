#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <glib.h>
#include <glib-object.h>
#include <xine.h>

G_BEGIN_DECLS

#define RF_MEDIA_XINE_TYPE            (rf_media_xine_get_type ())
#define RF_MEDIA_XINE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), RF_MEDIA_XINE_TYPE, RfMediaXine))
#define RF_MEDIA_XINE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), RF_MEDIA_XINE_TYPE, RfMediaXineClass))
#define IS_RF_MEDIA_XINE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RF_MEDIA_XINE_TYPE))
#define IS_RF_MEDIA_XINE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RF_MEDIA_XINE_TYPE))

struct _RfMediaXine {

	GtkWidget                   widget;

	GdkWindow                  *video_window;
	GdkWindowAttr               attributes;

	xine_t                     *xine;
	xine_stream_t              *stream;
	xine_video_port_t          *vo_port;
	xine_audio_port_t          *ao_port;
	xine_event_queue_t         *event_queue;
	gchar                      *vo_driver;
	gchar                      *ao_driver;
	
	gchar                       configfile[2048];
	x11_visual_t                vis;

/*
	GThread                    *thread;
	gboolean                    play;
*/
};

struct _RfMediaXineClass {

	GtkWidgetClass              parent_class;
	
};

typedef struct _RfMediaXine         RfMediaXine;
typedef struct _RfMediaXineClass    RfMediaXineClass;

GtkWidget* rf_media_xine_new (void);
GType rf_media_xine_get_type (void);

G_END_DECLS
