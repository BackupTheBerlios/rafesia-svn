//#define TICTACTOE(obj)          GTK_CHECK_CAST (obj, tictactoe_get_type (), Tictactoe)
//#define TICTACTOE_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, tictactoe_get_type (), RfMixerClass)
//#define IS_TICTACTOE(obj)       GTK_CHECK_TYPE (obj, tictactoe_get_type ())



typedef struct _RfMixerPrivate  RfMixerPrivate;
typedef struct _RfMixer         RfMixer;
typedef struct _RfMixerClass    RfMixerClass;


struct _RfMixerPrivate {

	GtkWidget *window;
	GtkWidget *button;
	GtkWidget *scale;
};
	

struct _RfMixer {

	GtkVBox parent;
	RfMixerPrivate *priv;
	
};

struct _RfMixerClass {
	GtkVBoxClass parent;
//	void (* rf_mixer) (RfMixer *ttt);
	
};

GType          rf_mixer_get_type             (void);
GtkWidget*     rf_mixer_new                  (void);
static void    rf_mixer_class_init           (RfMixerClass *klass);
static void    rf_mixer_init                 (RfMixer      *ttt);
static void    rf_mixer_toggle               (GtkWidget *widget, RfMixer *ttt);

enum {
	TICTACTOE_SIGNAL,
	LAST_SIGNAL
};
static gint rf_mixer_signals[LAST_SIGNAL] = { 0 };

static GtkVBoxClass *parent_class = NULL;

