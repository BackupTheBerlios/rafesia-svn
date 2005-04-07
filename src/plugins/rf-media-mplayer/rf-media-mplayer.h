#ifndef __RF_MEDIA_MPLAYER_H__
#define __RF_MEDIA_MPLAYER_H__

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <xine.h>

G_BEGIN_DECLS
#define RF_MEDIA_MPLAYER_TYPE            (rf_media_mplayer_get_type ())
#define RF_MEDIA_MPLAYER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), RF_MEDIA_MPLAYER_TYPE, RfMediaMplayer))
#define RF_MEDIA_MPLAYER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), RF_MEDIA_MPLAYER_TYPE, RfMediaMplayerClass))
#define IS_RF_MEDIA_MPLAYER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RF_MEDIA_MPLAYER_TYPE))
#define IS_RF_MEDIA_MPLAYER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RF_MEDIA_MPLAYER_TYPE))

struct _RfMediaMplayer {

	GtkWidget                   widget;
	GdkWindow                  *mp_window;
	
	gchar                      *file;
	gint                        status;
	gint                        width;
	gint                        height;
	gint                        length;
	gulong                      timer;
	gboolean                    ready;

	gint                        org_height;
	gint                        org_width;

	gint                        mp_in;
	FILE                       *stream_input;
	GIOChannel                 *channel_input;
	gint                        mp_out;
	GIOChannel                 *channel_output;
	
	gint                        mp_pid;
	guint32                     xid;
	guint                       watch_out_id;
};

struct _RfMediaMplayerClass {
	
	GtkSocketClass              parent_class;
	
};

typedef struct _RfMediaMplayer         RfMediaMplayer;
typedef struct _RfMediaMplayerClass    RfMediaMplayerClass;

GtkWidget* rf_media_mplayer_new (void);
GType rf_media_mplayer_get_type (void);

void rf_media_mplayer_open (GtkWidget *rmm, gchar *file);
gboolean rf_media_mplayer_is_running (GtkWidget *widget);

G_END_DECLS

#endif /* __RF_MEDIA_MPLAYER_H__ */
