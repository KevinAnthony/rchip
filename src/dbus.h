/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
*
*    rchip, Remote Controlled Home Integration Program
*    Copyright (C) 2011 <Kevin@NoSideRacing.com>
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

#ifndef                DBUS_H
#define                DBUS_H

#include            <gio/gio.h>
#include            "utils.h"

gboolean                    dbus_is_connected                               ( gboolean );
gboolean                    dbus_init                                       ( void );
gboolean                    send_command_to_music_player                    ( char* );    
gboolean                    send_command_to_music_player_with_argument      ( char*,char*, char* );
gboolean                    send_command_to_video_player                    ( char* );      
gboolean                    send_command_to_video_player_with_argument      ( char*,char*, char* );
struct playing_info_music   dbus_get_playing_info_music                     ( void );
gchar*                      get_hash_str                                    ( GHashTable*, const char* );    
unsigned int                get_hash_uint                                   ( GHashTable*, const char* );
void                        print_playing_info_music                        ( const struct playing_info_music );
void                        on_name_appeared                                ( GDBusConnection *connection, const gchar *name, const gchar *name_owner, gpointer user_data );
void                        on_name_vanished                                ( GDBusConnection*, const gchar*, gpointer );
#endif
