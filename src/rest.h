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

#ifndef JSON_H
#define JSON_H

#ifdef _JSON
#include			<curl/curl.h>
#include			<json/json.h>
#include			<glib.h>
#include			"utils.h"

void 				get_cmd_from_server					( char* );
size_t 				get_commands_callback				( void*,size_t,size_t,void*);
char* 				get_registered_devices_message		( void );
void 				get_active_devices					( void );
size_t 				get_active_decives_callback			( void*,size_t,size_t,void*);
void 				set_song_info_rest					( struct playing_info_music, char* );
void 				update_daemon_server				( void );
#endif //#ifdef _JSON
#endif
