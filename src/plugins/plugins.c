/*
 * Copyright (C) 2004 £ukasz ¯ukowski <pax@legar.pl>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
 
#include <gtk/gtk.h>
#include "plugins.h"
#include "../rafesia.h"

GtkWidget *rf_module_chooser_info;

void
rf_media_set_description (ModuleInfo *info, gchar *description) {

	g_assert (info != NULL);
	
	info->description = g_strdup (description);

}

void
rf_media_set_name (ModuleInfo *info, gchar *name) {

	g_assert (info != NULL);
	
	info->name = g_strdup (name);

}

void
rf_module_null_cb (void) {
	g_warning ("funkcja modulu niedostepna\n");
}

MediaModule *
rf_module_media_load_from_file (gchar *filename) {
	
	MediaModule     *mmod = g_new0 (MediaModule, 1);
	ModuleInfo      *(*get_module_info) (void);
	gint            (*init) (MediaModule *module);
	
	if (filename == NULL) 
		return NULL;
	
	mmod->filename = filename;
	
	mmod->module = g_module_open (mmod->filename, G_MODULE_BIND_LAZY);
	if (mmod->module == NULL) {
		g_printf ("%s\n", g_module_error() );
		return;
	}
	
	g_module_symbol (mmod->module, "get_module_info", (gpointer *)&get_module_info);
	if (get_module_info != NULL ) {
		mmod->info = get_module_info ();
		
		if (mmod->info == NULL)
			return (NULL);
		
		if (mmod->info->type != RF_MODULE_MEDIA)
			return (NULL);
			
		mmod->info->filename = mmod->filename;
		g_message ("module file: %s\n", mmod->info->filename);
		g_message ("Loading media module: %s\n", mmod->info->name);
		g_message ("(c) %s, rafesia req: %s\n", mmod->info->author, mmod->info->required_version);
		g_message ("%s\n", mmod->info->description);
	} else {
		return (NULL);
	}
	
	g_module_symbol (mmod->module, "media_module_init", (gpointer *)&init);
	if (init == NULL) {
		g_error ("cannot found init_plugin function\n");
		return (NULL);
	}

	init (mmod);

	g_module_symbol (mmod->module, "open", (gpointer *)&(mmod->open_mrl));
	if (mmod->open_mrl == NULL)
		mmod->open_mrl = (void *)&rf_module_null_cb;
	
	g_module_symbol (mmod->module, "play", (gpointer *)&(mmod->play));
	if (mmod->play == NULL)
		mmod->play = (void *)rf_module_null_cb;

	g_module_symbol (mmod->module, "stop", (gpointer *)&(mmod->stop));
	if (mmod->stop == NULL)
		mmod->stop = (void *)rf_module_null_cb;
	
	g_module_symbol (mmod->module, "rf_media_pause", (gpointer *)&(mmod->pause));
	if (mmod->pause == NULL)
		mmod->pause = (void *)rf_module_null_cb;
		
	g_module_symbol (mmod->module, "go", (gpointer *)&(mmod->go));
	if (mmod->go == NULL)
		mmod->go = (void *)rf_module_null_cb;
		
	g_module_symbol (mmod->module, "get_position", (gpointer *)&(mmod->get_position));
	if (mmod->get_position == NULL)
		mmod->get_position = (void *)rf_module_null_cb;
		
	g_module_symbol (mmod->module, "get_status", (gpointer *)&(mmod->get_status));
	if (mmod->get_status == NULL)
		mmod->get_status = (void *)rf_module_null_cb;
		
	g_module_symbol (mmod->module, "event_init", (gpointer *)&(mmod->event_init));
	if (mmod->event_init == NULL)
		mmod->event_init = (void *)rf_module_null_cb;
	
	return (mmod);
	
}

void 
rf_add_plugins_to_list (GSList *plugins, GtkWidget *treeview) {

	GtkTreeIter       iter;
	GtkTreeStore     *list_model = GTK_TREE_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (treeview)));
	
	for ( ; plugins != NULL; plugins=g_slist_next (plugins) ) {
		ModuleInfo    *info = (ModuleInfo *) plugins->data;

		gtk_tree_store_append (list_model, &iter, NULL);
		gtk_tree_store_set (list_model, &iter, 0, info->name, 1, info, -1);
	}

}

gboolean
rf_module_chooser_click (GtkWidget *widget, GdkEventButton *event, gpointer user_data) {

	GtkTreeViewColumn	*treevc	= NULL;
	GtkTreePath		*treepath = NULL;
	GtkTreeModel 		*model = gtk_tree_view_get_model(GTK_TREE_VIEW(widget));
	GtkTreeIter 		 iter;


	switch (event->type) {
		case GDK_2BUTTON_PRESS: {
			ModuleInfo    *info;
			MediaModule   *mmod;
			gchar         *tmp;
			gint           i;
			
			if (!gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW(widget), event->x, event->y, &treepath, &treevc, NULL, NULL)) 
				return (FALSE);
			
			gtk_tree_model_get_iter (model,&iter,treepath);
			gtk_tree_view_set_cursor (GTK_TREE_VIEW (widget), treepath, 0, FALSE);
			gtk_tree_model_get(model, &iter, 1, &info, -1);
			
			mmod = rf_module_media_load_from_file (info->filename);
			rf_set_module_media (mmod);
			gtk_widget_destroy (GTK_WIDGET (user_data));
			
			break;
		}
		case GDK_BUTTON_PRESS: {
			ModuleInfo    *info;
			
			if (!gtk_tree_view_get_path_at_pos (GTK_TREE_VIEW (widget), event->x, event->y, &treepath, &treevc, NULL,NULL)) 
				return (FALSE);
			
			gtk_tree_model_get_iter (model,&iter,treepath);
			gtk_tree_view_set_cursor (GTK_TREE_VIEW (widget), treepath, 0, FALSE);
			gtk_tree_model_get (model, &iter, 1, &info, -1);

			gtk_label_set_text (GTK_LABEL (rf_module_chooser_info), g_strdup_printf ("Module file: %s\n(c) %s\n%s", info->filename, info->author, info->description));
		}
	}
	
	return (TRUE);
	
}

void
rf_module_chooser(GSList *plugins) {

	GtkWidget *chooser_window;
	GtkWidget *vbox;
	GtkWidget *title;
	GtkWidget *scrolledwindow;
	
	GtkWidget *list;
	GtkTreeStore *list_model = NULL;
	GtkCellRenderer *renderer_pixbuf, *renderer_text;
	GtkTreeViewColumn *column;
	
	GtkWidget *hbox;
	GtkWidget *image;
	GtkWidget *info;

	chooser_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width (GTK_CONTAINER (chooser_window), 5);
	gtk_window_set_title (GTK_WINDOW (chooser_window), "Module Chooser");
	g_signal_connect (GTK_OBJECT (chooser_window), "delete_event", G_CALLBACK (rafesia_quit),NULL);
	g_signal_connect (GTK_OBJECT (chooser_window), "destroy_event", G_CALLBACK (rafesia_quit),NULL);

	vbox = gtk_vbox_new (FALSE, 5);
	gtk_widget_show (vbox);
	gtk_container_add (GTK_CONTAINER (chooser_window), vbox);

	title = gtk_label_new ("Module Chooser\nPlease choose one of the following media modules:");
	gtk_widget_show (title);
	gtk_box_pack_start (GTK_BOX (vbox), title, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (title), GTK_JUSTIFY_CENTER);

	scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (scrolledwindow);
	gtk_box_pack_start (GTK_BOX (vbox), scrolledwindow, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_SHADOW_ETCHED_IN);

	list_model = gtk_tree_store_new (2, G_TYPE_STRING, G_TYPE_POINTER);
	list = gtk_tree_view_new ();
  
	gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (list)), GTK_SELECTION_MULTIPLE);
	gtk_tree_view_columns_autosize (GTK_TREE_VIEW (list));
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (list), FALSE);
	gtk_tree_view_set_rules_hint (GTK_TREE_VIEW(list), FALSE);
	
	column = gtk_tree_view_column_new ();
	gtk_tree_view_column_set_title (column, "Modules");
	
	renderer_text = gtk_cell_renderer_text_new ();
	gtk_tree_view_column_pack_start (column, renderer_text, TRUE);
	gtk_tree_view_column_set_attributes (column, renderer_text, "text", 0, NULL);
	gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN (column), GTK_TREE_VIEW_COLUMN_AUTOSIZE);
	
	gtk_tree_view_set_headers_visible( GTK_TREE_VIEW(list), FALSE);
	gtk_tree_view_append_column (GTK_TREE_VIEW(list), column); 
	gtk_tree_view_set_model (GTK_TREE_VIEW(list), GTK_TREE_MODEL(list_model));
	g_object_unref (list_model);
	
	gtk_widget_show (list);
	gtk_container_add (GTK_CONTAINER (scrolledwindow), list);
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (list), FALSE);	

	rf_add_plugins_to_list (plugins, list);
	g_signal_connect (G_OBJECT(list), "button-release-event", G_CALLBACK (rf_module_chooser_click), chooser_window);
	g_signal_connect (G_OBJECT(list), "button-press-event", G_CALLBACK (rf_module_chooser_click), chooser_window);

	hbox = gtk_hbox_new (FALSE, 5);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (hbox), 5);
	
	image = gtk_image_new_from_stock ("gtk-execute", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show (image);
	gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, FALSE, 0);
	
	rf_module_chooser_info = gtk_label_new (NULL);
	gtk_widget_show (rf_module_chooser_info);
	gtk_box_pack_start (GTK_BOX (hbox), rf_module_chooser_info, FALSE, TRUE, 0);
	gtk_widget_show_all (chooser_window);
}

gchar *
rf_media_module_search (gchar *path) {

	GSList     *plugins = NULL;
	gchar      *file = NULL;
	GDir       *plug_dir = g_dir_open (path, 0, NULL);
	
	for (file = (gchar *)g_dir_read_name (plug_dir); file != NULL; file= (gchar *) g_dir_read_name (plug_dir))
		if (g_str_has_suffix (file, ".so") == TRUE) {
			ModuleInfo      *(*get_module_info) (void);
			GModule         *module;
			ModuleInfo      *info;
			gchar           *filename = g_build_filename (path, file, NULL);
			
			module = g_module_open (filename, G_MODULE_BIND_LAZY);
			if (module == NULL)
				continue;
			
			g_module_symbol (module, "get_module_info", (gpointer *)&get_module_info);
			if (get_module_info == NULL)
				continue;
			
			info = get_module_info ();
			if (info == NULL)
				continue;
			
			if (info->type != RF_MODULE_MEDIA)
				continue;
			
			info->filename = g_strdup (filename);
			plugins = g_slist_append (plugins, info);
		}

	g_dir_close(plug_dir);

	switch (g_slist_length (plugins)) {
		case (0):
			return (NULL);
		case (1): {
			ModuleInfo *info = (ModuleInfo *) plugins->data;
			return (info->filename);
		}
		default:
			rf_module_chooser (plugins);
			return (NULL);
	}

}

MediaModule *
rf_module_media_load (gchar *path, gchar *file) {
	
	gchar           *filename;
	
	if (path == NULL)
		return (NULL);

	if (file != NULL) {
		filename = g_strdup_printf ("%s/%s", path, file);
		return (rf_module_media_load_from_file (filename));
	}
		
	filename = rf_media_module_search (path);
	if (filename == NULL)
		return (NULL);
	
	return (rf_module_media_load_from_file (filename));
	
}

