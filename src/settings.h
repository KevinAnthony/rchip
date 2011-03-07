/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
*
*    rchip, Remote Controlled Home Integration Program
*    Copyright (C) 2011 <Kevin@NoSideRacing.com
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#ifndef SETTINGS_H
#define SETTINGS_H

#ifdef _WIN32
	#include <libxml/parser.h>
	#include <libxml/tree.h>
	#define XMLFILE PREFIX"/share/settings.xml"
	char* get_setting(char*);
	char* find_element_names(xmlNode*,char*);
	int xml_file_exists();
	int new_xml_file();

#else
	#include <glib.h>
	#include <gconf/gconf-client.h>

	#define SERVICE_GCONF_ROOT "/apps/noside/msdaemon"
	
	#define VIDEO_ROOT "/videoroot"
	#define SERVICE_KEY_PATH_TO_VIDEO_ROOT \
	        SERVICE_GCONF_ROOT "/videoroot"
	
	void key_change_callback(GConfClient*,guint,GConfEntry*,gpointer);
	gboolean register_callback( void (*) (void),gchar*);
	gboolean unregister_callback( void (*) (gchar*));
	gboolean callback_registered_functions(gchar*);
	char* get_setting( gchar*);
	void populate_defaults(GConfClient*);
	gboolean settings_init();

#endif // # ifdef _WIN32
#endif
