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
#include "rf-media-mplayer.h"
#include "../plugins.h"

gint
open (MediaModule *module, gchar *mrl) {
	
	RfMediaMplayer *rmm;
	
	g_return_if_fail (module != NULL);
	g_return_if_fail (module->widget != NULL);
	g_return_if_fail (IS_RF_MEDIA_MPLAYER (module->widget));

	rmm = RF_MEDIA_MPLAYER (module->widget);
	
	rf_media_mplayer_open (module->widget, mrl);
	rmm->status = RF_STATUS_PLAY;
	return 0;

}

gint 
play (MediaModule *module) {
	
	RfMediaMplayer *rmm;
	
	g_return_if_fail (module != NULL);
	g_return_if_fail (module->widget != NULL);
	g_return_if_fail (IS_RF_MEDIA_MPLAYER (module->widget));

	rmm = RF_MEDIA_MPLAYER (module->widget);	
	
	g_io_channel_write_chars (rmm->channel_input, "seek 0 0\n", -1, NULL, NULL);
	g_io_channel_flush (rmm->channel_input, NULL);
	rmm->status = RF_STATUS_PLAY;
	
	return 0;
	
}

gint 
stop (MediaModule *module) {
	
	return 0;
	
}

gint
rf_media_pause (MediaModule *module) {
	
	RfMediaMplayer *rmm;
	
	g_return_if_fail (module != NULL);
	g_return_if_fail (module->widget != NULL);
	g_return_if_fail (IS_RF_MEDIA_MPLAYER (module->widget));

	rmm = RF_MEDIA_MPLAYER (module->widget);
	
	g_io_channel_write_chars (rmm->channel_input, "pause\n", -1, NULL, NULL);
	g_io_channel_flush (rmm->channel_input, NULL);
	rmm->status = RF_STATUS_PAUSE;
	
	return 0;
	
}

gint
go (MediaModule *module, gint pos_stream, gint pos_time, gboolean actual) {
	
	RfMediaMplayer *rmm;
	
	g_return_if_fail (module != NULL);
	g_return_if_fail (module->widget != NULL);
	g_return_if_fail (IS_RF_MEDIA_MPLAYER (module->widget));

	rmm = RF_MEDIA_MPLAYER (module->widget);
	
	switch (actual) {
		case 0:
			if (pos_stream == 0) {
				
				gchar *cmd = g_strdup_printf ("seek %d 2\n", pos_time/1000);
				
				g_io_channel_write_chars (rmm->channel_input, cmd, -1, NULL, NULL);
				g_io_channel_flush (rmm->channel_input, NULL);
				g_free (cmd);
				
			} else {
				
				gint pos_t = (pos_stream * rmm->length) / 65536;
				gchar *cmd = g_strdup_printf ("seek %d 2\n", pos_t);
				
				g_io_channel_write_chars (rmm->channel_input, cmd, -1, NULL, NULL);
				g_io_channel_flush (rmm->channel_input, NULL);
				g_free (cmd);
				
			}
			
			break;
		case 1:
			if (pos_stream == 0) {
				
				gchar *cmd = g_strdup_printf ("seek %d 0\n", pos_time/1000);

				g_io_channel_write_chars (rmm->channel_input, cmd, -1, NULL, NULL);
				g_io_channel_flush (rmm->channel_input, NULL);
				g_free (cmd);

			} else {
				
				gint pos_t = (pos_stream * rmm->length) / 65536;
				gchar *cmd = g_strdup_printf ("seek %d 0\n", pos_t);
				
				g_io_channel_write_chars (rmm->channel_input, cmd, -1, NULL, NULL);
				g_io_channel_flush (rmm->channel_input, NULL);
				g_free (cmd);
				
			}
			
			break;
	}
	
	rmm->status = RF_STATUS_PLAY;
	
	return 0;
	
}

gint 
get_position (MediaModule *module, guint *pos_stream, guint *pos_time, guint *time) {
	
	RfMediaMplayer      *rmm = RF_MEDIA_MPLAYER (module->widget);
	
	if (rf_media_mplayer_is_running (GTK_WIDGET (module->widget))) {
		
		*pos_stream = (65536 / rmm->length + 1) * rmm->timer;
		*pos_time = rmm->timer;
		*time = rmm->length;
		
	} else {
		
		*pos_stream = 0;
		*pos_time = 0;
		*time = 0;

	}
	
	return TRUE;
}

gint
get_status (MediaModule *module) {
	
	RfMediaMplayer *rmm = RF_MEDIA_MPLAYER (module->widget);
	
	return rmm->status;
	
	if (rmm->ready)
		return RF_STATUS_STOP;
	else
		return RF_STATUS_PLAY;
}

gint
event_init (void *ptr_media, void (*media_event_cb)(gint event)) {
	
	/*
	 RfMediaXine     *media = RF_MEDIA_XINE(ptr_media);
	 
	 xine_event_create_listener_thread (xine_event_new_queue (media->stream), xine_event_cb, media_event_cb);
	 */
	return 0;

}

ModuleInfo *
get_module_info (gint (*ModuleInfoFunc) (ModuleInfo *info, gchar *name, gpointer var)) {
	
	ModuleInfo *info = g_new0 (ModuleInfo, 1);

	ModuleInfoFunc (info, "name", "Rafesia Media Module [MPlayer]");
	ModuleInfoFunc (info, "description", "Alpha phase mplayer plugin for rafesia 0.0.0");
	ModuleInfoFunc (info, "author", "Luke 'pax' Zukowski");
	ModuleInfoFunc (info, "required version", "=0.0.0");
	ModuleInfoFunc (info, "type", RF_MODULE_MEDIA);
	
	return info;
	
}

gint
media_module_init (MediaModule *module) {

	g_return_val_if_fail (module != NULL, -1);
	
	module->widget = rf_media_mplayer_new ();
	
	return 0;
	
}

