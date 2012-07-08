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
#include            <curl/curl.h>
#include            <json/json.h>
#include            <glib.h>
#include            "utils.h"

void                rest_init                           ( void );
void                rest_thread_handler                 ( gpointer* );
gpointer*           get_cmd_from_server                 ( gpointer* );
gpointer*           send_cmd_to_server                  ( gpointer* );
size_t              get_commands_callback               ( void*,size_t,size_t,void*);
char*               get_registered_devices_message      ( void );
gpointer*           get_active_devices                  ( gpointer* );
size_t              get_active_decives_callback         ( void*,size_t,size_t,void*);
gpointer*           set_song_info_rest                  ( gpointer* );
void                update_daemon_server                ( void );
void                authenticate                        ( void );
void                deauthenticate                      ( void );

typedef struct CommandData{
    char* hostname;
    char* command;
    char* command_text;
} command_data;

typedef struct SongInfoData{
    struct playing_info_music pInfo;
    char* hostname;
} song_info_data;

#endif //#ifdef _JSON
#endif
