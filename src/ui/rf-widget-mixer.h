#ifndef __RF_WIDGET_MIXER_H__
#define __RF_WIDGET_MIXER_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS
#define RF_MIXER_TYPE            (rf_mixer_get_type ())
#define RF_MIXER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), RF_MIXER_TYPE, RfMixer))
#define RF_MIXER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), RF_MIXER_TYPE, RftMixerClass))
#define IS_RF_MIXER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RF_MIXER_TYPE))
#define IS_RF_MIXER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RF_MIXER_TYPE))

struct _RfMixer {

	GtkButton                   button;
	GtkWidget                  *window;
	GtkWidget                  *scale;

};

struct _RfMixerClass {

	GtkButtonClass              parent_class;
	
};

typedef struct _RfMixer         RfMixer;
typedef struct _RfMixerClass    RfMixerClass;

GtkWidget* rf_mixer_new (void);
GType rf_mixer_get_type (void);

G_END_DECLS

#endif /* __RF_WIDGET_MIXER_H__ */
