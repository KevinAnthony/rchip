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
#include <config.h>

#include <glib.h>
#include <glib/gprintf.h>
#include <stdlib.h>
#include <string.h>
/*for hostname on unix and unix-like systems*/
#include <sys/utsname.h>
#ifndef DBUSOFF
/*windows doesn't have a dbus so we can't use it*/
#include "dbus.h"
#endif
#endif
#include "cmdhandler.h"
#include "settings.h"
#include "utils.h"
#include "xml.h"

/*
 * (insert mad scientist type laughter here)
 */

void get_next_cmd() {
    /*uts.nodename is the hostname of the computer*/
    struct utsname uts;
	uname( &uts );
	get_cmd_from_server(uts.nodename);
}

gboolean process_cmd(char* cmd,char* cmdTxt) {
	gboolean xmli = xml_init();
	gboolean xmlf = xml_find_command(cmd);
	if (!xmli){
		g_printf("XML_INIT PROBLEM\n");
		return FALSE;
	}
	if (!xmlf){
		g_printf("XML_FIND_COMMAND(%s) PROBLEM\n",cmd);
		return FALSE;
	}
	if (xmli && xmlf){
		char* type = xml_get_type();
		char* musicOrVideo = xml_get_music_or_video();
		if(g_strcmp0(type,"SYSTEM") == 0){
			if (g_strcmp0(cmdTxt,"") != 0){
				cmdTxt = g_strescape(cmdTxt,"");
				gchar* newCmdTxt = malloc(strlen(cmdTxt)*2);
				char* p = cmdTxt;
				char* q = newCmdTxt;
				int len = 0;
				while (*p != '\0'){
					//if ((*p == '\'') || (*p == '\"') || (*p == ' ')){
					if (*p == ' '){
						*q = '\\';
						q++;
						len++;
					}
					*q=*p;
					p++;
					q++;
					len++;
				}
				g_free(cmdTxt);
				cmdTxt = g_strndup(newCmdTxt,len);
				g_free(newCmdTxt);
			}
			gchar* command = g_strdup_printf ("%s %s&",xml_get_system_command(),cmdTxt);
			g_printf("%s\n",command);
			system(command);
			g_free(command);
		} else if (g_strcmp0(type,"DBUS") == 0) {
			char* argument = xml_get_dbus_argument();
			char* argument_str = xml_get_dbus_argument_type();
			char* dbus_command = xml_get_dbus_command();
			if (argument != NULL){
				if(g_strcmp0(musicOrVideo,"MUSIC") == 0){
					send_command_to_music_player_with_argument(dbus_command,argument_str,argument);
				} else {
					send_command_to_video_player_with_argument(dbus_command,argument_str,argument);
				}
				g_free(argument);
				g_free(argument_str);
			} else {
				if(g_strcmp0(musicOrVideo,"MUSIC") == 0){
					send_command_to_music_player(dbus_command);
				} else {
					send_command_to_video_player(dbus_command);
				}
			}
			g_free(dbus_command);
		}
		g_free(type);
		return TRUE;
	} else {
		return FALSE;
	}
}

/* insert command and command text into cmdQueue, once per hsotname*/
void send_cmd(char* cmd, char* cmdTxt) {
}
