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

#include <gmodule.h>

enum {
	RF_STATUS_STOP,
	RF_STATUS_PLAY,
	RF_STATUS_PAUSE,
	RF_STATUS_EMPTY,
	RF_STATUS_QUIT
};

enum {
	RF_EVENT_PLAYBACK_FINISHED,
	RF_EVENT_MOUSE_MOVE,
	RF_EVENT_MOUSE_CLICK_1
};

enum {
	RF_MODULE_MEDIA,
	RF_MODULE_PLUGIN
};

typedef struct _ModuleInfo ModuleInfo;
struct _ModuleInfo {
	gint                     type;
	gchar                   *name;
	gchar                   *author;
	gchar                   *description;
	gchar                   *required_version;
	gchar                   *filename;
};

typedef struct _MediaModule MediaModule;
struct _MediaModule {
	void                          *widget;
	
	GModule                       *module;
	gchar                         *filename;
	ModuleInfo                    *info;

	gint (*open_mrl)             (MediaModule *module, gchar *mrl);
	gint (*play)                 (MediaModule *module);
	gint (*stop)                 (MediaModule *module);
	gint (*pause)                (MediaModule *module);
	gint (*go)                   (MediaModule *module, gint pos_stream, gint pos_time, gboolean actual);
	gint (*get_position)         (MediaModule *module, gint *pos_stream, gint *pos_time, gint *time);
	gint (*get_status)           (MediaModule *module);
	gint (*event_init)           (void *mwidget, void (*media_event_cb)(gint event));
	void (*set_fullscreen)       (MediaModule *module, gint fullscreen);
	gboolean (*is_fullscreen)    (MediaModule *module);
};

MediaModule *rf_module_media_load (gchar *path, gchar *file);
void rf_media_set_description (ModuleInfo *info, gchar *description);
void rf_media_set_name (ModuleInfo *info, gchar *name);

