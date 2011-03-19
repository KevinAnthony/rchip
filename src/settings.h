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

#ifndef 			SETTINGS_H
#define 			SETTINGS_H

#include 			<glib.h>
#include			<gio/gio.h>

#define 			VIDEO_ROOT 					"raidroot"
#define 			SQL_SERVER 					"sql-server"
#define				SQL_USERNAME					"sql-username"
#define				SQL_PASSWORD					"sql-passwd"
#define				SQL_DATABASE					"sql-db"
#define				SQL_MAX_NAME					"sql-hostnamesize"
#define				MUSIC_XML					"xml-music"
#define				VIDEO_XML					"xml-video"

char* 				get_setting_str					( gchar* );
gboolean			set_setting_str					( gchar*,gchar* );
int				get_setting_int					( gchar* );
gboolean			set_setting_int					( gchar*,int );
void				settings_unref					( void );
void				setting_changed					( GSettings*, gchar*, gpointer );
gboolean 			settings_init					( void );

#endif
