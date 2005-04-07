#ifndef __RF_WIDGET_MCONTAINER_H__
#define __RF_WIDGET_MCONTAINER_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS
#define RF_MCONTAINER_TYPE            (rf_mcontainer_get_type ())
#define RF_MCONTAINER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), RF_MCONTAINER_TYPE, RfMContainer))
#define RF_MCONTAINER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), RF_MCONTAINER_TYPE, RfMContainerClass))
#define IS_RF_MCONTAINER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), RF_MCONTAINER_TYPE))
#define IS_RF_MCONTAINER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), RF_MCONTAINER_TYPE))

struct _RfMContainer {

	GtkContainer                  container;
	GtkWidget                    *child;

};

struct _RfMContainerClass {

	GtkContainerClass             parent_class;
	
};

typedef struct _RfMContainer          RfMContainer;
typedef struct _RfMContainerClass     RfMContainerClass;

GtkWidget *rf_mcontainer_new (void);
GType rf_mcontainer_get_type (void);

G_END_DECLS

#endif /* __RF_WIDGET_MCONTAINER_H__ */
