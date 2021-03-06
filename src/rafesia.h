/*
 * Copyright (C) 2004 �ukasz �ukowski <pax@legar.pl>
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

#define VERSION "0.0.1"

#define RF_NORMAL  "\e[0m"
#define RF_WHITE   "\e[0;37m"
#define RF_YELLOW  "\e[0;33m"
#define RF_GREEN   "\e[0;32m"
#define RF_RED     "\e[0;31m"
#define RF_YELLOW  "\e[0;33m"
#define RF_BLUE    "\e[0;34m"
#define RF_MAGENTA "\e[0;35m"
#define RF_BOLD    RF_YELLOW

GtkWidget *rf_widget_get (gchar *name);
gboolean   rf_widget_add (GtkWidget *widget, gchar *name);
gboolean   rf_widget_remove (gchar *name);


void rf_set_module_media (void *mmod);
void rafesia_quit();
