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
#include <xine.h>
#include "rf-media-xine.h"
#include "../plugins.h"

gint
open(MediaModule *module, gchar *mrl) {

	RfMediaXine     *media = RF_MEDIA_XINE (module->widget);
	
	return xine_open (media->stream, mrl);

}

gint 
play (MediaModule *module) {
	
	RfMediaXine     *media = RF_MEDIA_XINE (module->widget);
	gint             ps=0;
	
	xine_get_pos_length (media->stream, &ps, NULL, NULL);
	xine_play (media->stream, ps, 0);
	xine_set_param (media->stream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL);
	
	return (0);
	
}

gint 
stop (MediaModule *module) {
	
	RfMediaXine     *media = RF_MEDIA_XINE (module->widget);

	xine_stop (media->stream);

	return 0;
	
}

gint
rf_media_pause (MediaModule *module) {
	
	RfMediaXine     *media = RF_MEDIA_XINE (module->widget);

	xine_set_param (media->stream, XINE_PARAM_SPEED, XINE_SPEED_PAUSE);

	return 0;
	
}

gint
go (MediaModule *module, gint pos_stream, gint pos_time, gboolean actual) {
	
	RfMediaXine        *media = RF_MEDIA_XINE (module->widget);
	gint                ps = 0, pt = 0;
	
	switch (actual) {
		case 0:
			xine_play (media->stream, pos_stream, pos_time);
			xine_set_param (media->stream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL);
			xine_set_param (media->stream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL);
			break;
		case 1:
			xine_get_pos_length (media->stream, &ps, &pt, NULL);
			if (pos_stream == 0)
				xine_play (media->stream, 0, pt+pos_time);
			else
				xine_play (media->stream, ps+pos_stream, 0);
				xine_set_param (media->stream, XINE_PARAM_SPEED, XINE_SPEED_NORMAL);
			break;
	}
	
	return (0);
	
}

gint 
get_position (MediaModule *module, gint *pos_stream, gint *pos_time, gint *time) {
	
	RfMediaXine      *media = RF_MEDIA_XINE (module->widget);
	
	return xine_get_pos_length (media->stream, pos_stream, pos_time, time);
	
}

gint
get_status (MediaModule *module) {

	RfMediaXine     *media = RF_MEDIA_XINE(module->widget);

	switch ( xine_get_status (media->stream) ) {
		case XINE_STATUS_IDLE:
			return RF_STATUS_EMPTY;
		case XINE_STATUS_STOP:
			return RF_STATUS_STOP;
		case XINE_STATUS_PLAY:
			switch (xine_get_param (media->stream, XINE_PARAM_SPEED)) {
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

	void     (*media_event_cb) (gint event);

	media_event_cb = user_data;

	switch (event->type) {
		case XINE_EVENT_UI_PLAYBACK_FINISHED:
			media_event_cb (RF_EVENT_PLAYBACK_FINISHED);
			break;
		case XINE_EVENT_DROPPED_FRAMES:
			break;
		default:
			media_event_cb (event->type);
	}

}

gint
event_init (void *ptr_media, void (*media_event_cb)(gint event)) {
	
	RfMediaXine     *media = RF_MEDIA_XINE(ptr_media);

	xine_event_create_listener_thread (xine_event_new_queue (media->stream), xine_event_cb, media_event_cb);
	
	return 0;

}

ModuleInfo *
get_module_info () {
	ModuleInfo *info = g_new0(ModuleInfo, 1);

	rf_media_set_name (info, "Rafesia Media Module [XinE]");
	rf_media_set_description (info, "Alpha phase xine plugin for rafesia 0.0.0");
	
	/* potem mozna to dokonczyc, nie jest to sprawa pierwszorzedna */
	info->type = RF_MODULE_MEDIA;

	info->author = g_strdup ("Lukasz 'pax' Zukowski");
	info->required_version = g_strdup ("=0.0.0");

	return info;
}

gint
media_module_init (MediaModule *module) {
	g_return_val_if_fail (module != NULL, -1);
	
	module->widget = rf_media_xine_new ();
	
	return 0;
	
}

