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

#ifndef 			NOTIFICATIONS_H
#define 			NOTIFICATIONS_H

#include			"sql.h"
#include			<glib.h>

#ifndef 			_WIN32
#include        		<dbus/dbus.h>
#include        		<dbus/dbus-glib.h>
#endif

gboolean 			set_notification				( char*,char*,char* );

#ifndef 			_WIN32
void 				get_torrent_info				( DBusGProxy*,char* );
#endif

#endif
