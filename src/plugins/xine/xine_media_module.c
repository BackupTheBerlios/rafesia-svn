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
#include "gtkxine.h"
#include "../plugins.h"

gint
open(MediaModule *module, gchar *mrl) {

	return gtk_xine_open(GTK_XINE(module->widget), mrl);
	
}

gint 
play (MediaModule *module) {

	gint ps=0, pt=0;
	
	gtk_xine_get_pos_length(GTK_XINE(module->widget), &ps, &pt, NULL);
	gtk_xine_play(GTK_XINE(module->widget), ps, pt);
	gtk_xine_set_param(GTK_XINE(module->widget), XINE_PARAM_SPEED, XINE_SPEED_NORMAL);
	
	return (0);
	
}

gint 
stop (MediaModule *module) {

	gtk_xine_stop(GTK_XINE(module->widget));
	
	return (0);
	
}

gint
rf_media_pause (MediaModule *module) {

	gtk_xine_set_param(GTK_XINE(module->widget), XINE_PARAM_SPEED, XINE_SPEED_PAUSE);
	
	return (0);
	
}

gint
go (MediaModule *module, gint pos_stream, gint pos_time, gboolean actual) {
	gint ps=0, pt=0;
	switch (actual) {
		case 0:
			gtk_xine_play(GTK_XINE(module->widget), pos_stream, pos_time);
			gtk_xine_set_param(GTK_XINE(module->widget), XINE_PARAM_SPEED, XINE_SPEED_NORMAL);
			gtk_xine_set_param(GTK_XINE(module->widget), XINE_PARAM_SPEED, XINE_SPEED_NORMAL);
			break;
		case 1:
			gtk_xine_get_pos_length(GTK_XINE(module->widget), &ps, &pt, NULL);
			if (pos_stream == 0)
				gtk_xine_play(GTK_XINE(module->widget), 0, pt+pos_time);
			else
				gtk_xine_play(GTK_XINE(module->widget), ps+pos_stream, 0);
			gtk_xine_set_param(GTK_XINE(module->widget), XINE_PARAM_SPEED, XINE_SPEED_NORMAL);
			break;
	}
	
	return (0);
	
}

gint 
get_position (MediaModule *module, gint *pos_stream, gint *pos_time, gint *time) {

	return gtk_xine_get_pos_length(GTK_XINE(module->widget), pos_stream, pos_time, time);
	
}

gint
get_status (MediaModule *module) {
	switch (gtk_xine_get_status(GTK_XINE(module->widget))) {
		case XINE_STATUS_IDLE:
			return RF_STATUS_EMPTY;
		case XINE_STATUS_STOP:
			return RF_STATUS_STOP;
		case XINE_STATUS_PLAY:
			switch (gtk_xine_get_param(GTK_XINE(module->widget), XINE_PARAM_SPEED)) {
				case XINE_SPEED_PAUSE:
					return RF_STATUS_PAUSE;
				case XINE_SPEED_NORMAL:
					return RF_STATUS_PLAY;
			}
		case XINE_STATUS_QUIT:
			return RF_STATUS_QUIT;
	}
	return (-1);
}

void
xine_event_cb (void *user_data, const xine_event_t *event) {
	void     (*media_event_cb)(gint event);

	media_event_cb = user_data;

	switch (event->type) {
		case XINE_EVENT_UI_PLAYBACK_FINISHED:
			media_event_cb(RF_EVENT_PLAYBACK_FINISHED);
			break;
	}

}

gint
event_init (GtkXine *gxine, void (*media_event_cb)(gint event)) {

	xine_event_create_listener_thread (xine_event_new_queue (gxine->stream), xine_event_cb, media_event_cb);
	
	return (0);
	
}

ModuleInfo *
get_module_info () {

	ModuleInfo *info = g_new0(ModuleInfo, 1);

	info->type=RF_MODULE_MEDIA;
	info->name=g_strdup("Xine");
	info->author=g_strdup("Lukasz 'pax' Zukowski");
	info->description=g_strdup("Alpha phase xine plugin for rafesia 0.0.0");
	info->required_version=g_strdup("=0.0.0");
	info->filename=NULL;

	return (info);
	
}

gint
media_module_init (MediaModule *module) {

	g_return_val_if_fail (module != NULL, -1);
	
	module->widget = gtk_xine_new ("auto", "oss");
	
	return (0);
	
}

