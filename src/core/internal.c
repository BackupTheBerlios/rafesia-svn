/*
 * Copyright (C) 2004 £ukasz ¯ukowski <pax@legar.pl>
 *                    Piotr Tarasewicz <ptw@hurd.pl>
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

#include <stdio.h>
#include <string.h>
#include <glib.h>

#include "internal.h"

gchar *
int_to_timestring (gint int_time, gint length) {
	
	gint hours, minutes, seconds;
	gchar *buf;
	seconds = int_time % 60;
	int_time /= 60;

	minutes = int_time % 60;
	int_time /= 60;

	hours = int_time;
	buf = g_strdup_printf ("%d:%02d:%02d", hours, minutes, seconds);

	return (buf);

}
