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
	
	gchar                      *file;
	
	gint                        pipe_input[2];
	gint                        pipe_output[2];
	gint                        mp_pid;
	guint32                     xid;

	gint                        width;
	gint                        height;
	gulong                      timer;
	gboolean                    ready;

};

struct _RfMediaMplayerClass {
	
	GtkSocketClass              parent_class;
	
};

typedef struct _RfMediaMplayer         RfMediaMplayer;
typedef struct _RfMediaMplayerClass    RfMediaMplayerClass;

GtkWidget* rf_media_mplayer_new (void);
GType rf_media_mplayer_get_type (void);

void rf_media_mplayer_play (GtkWidget *rmm, gchar *file);

G_END_DECLS

#endif /* __RF_MEDIA_MPLAYER_H__ */
