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
#ifndef _WIN32
	/*for hostname on unix and unix-like systems*/
	#include <sys/utsname.h>
	#ifndef DBUSOFF
	/*windows doesn't have a dbus so we can't use it*/
	#include "dbus.h"
	#endif
#else
	/*for hostname on windows machines*/
	#include <windows.h>
	#include <process.h>
	#define myerrno WSAGetLastError()
#endif
#include "sql.h"
#include "cmdhandler.h"
#include "settings.h"
#include "utils.h"
#include "xml.h"

/* returns the next hostname, from a pointer based array */
char* get_next_host(void* base,size_t size, size_t next){
	return base+(next*size);
}
/*
 * (insert mad scientist type laughter here)
 */

void get_next_cmd() {
	char* cmd = NULL;
        char* cmdTxt = NULL;
        char* source = NULL;
        int cmdID = 0;
        #ifndef _WIN32
                /*uts.nodename is the hostname of the computer*/
                struct utsname uts;
                uname( &uts );
                #ifdef _SQL
                get_next_cmd_from_sql(uts.nodename,&cmdID,&cmd,&cmdTxt,&source);
                #endif
        #else
                /*hn is allowed to be 500char plus 1 for the \0 char*/
                char hn[500 + 1];
                DWORD dwLen = 500;
                GetComputerName(hn, &dwLen);
                #ifdef _SQL
                get_next_cmd_from_sql(hn,&cmdID,&cmd,&cmdTxt,&source);
                #endif
        #endif
	if (cmd != NULL){
		
	
        	#if VERBOSE >= 4
                g_print("ID:%i\ncmd:%s:\ncmdTxt:%s:\n",cmdID,cmd,cmdTxt);
        	#endif	
		if(!process_cmd(cmd,cmdTxt)){
			#if VERBOSE >= 1
			g_error("process_cmd:something went wrong\n");
			#endif
		} else {
			delete_from_cmdQueue(cmdID);
		}
		g_free(cmd);
		g_free(source);
		g_free(cmdTxt);
	}
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
					if ((*p == '\'') || (*p == '\"') || (*p == ' ')){
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
	#ifdef _SQL
	/* Number of elements in array */
	size_t nelem = get_nelem();
	/* Size (in bytes) of each element */
	size_t size = get_size();   
	/* Size (in bytes) of each element */
	char* base;
	/* allocate array to size number of elements * size of elements */
	base = g_malloc(nelem*size);
	/* get list of active devices */
	get_active_devices(base,size,nelem);
	char* deviceName = g_malloc(size);
		for (int i = 0; i < nelem; i++) {
			/* current element = (itterator*size)+initial memeory location */
			char* elem = base+(i*size); 
			g_strlcpy(deviceName,elem,size);
			char* query = g_strdup_printf("INSERT INTO cmdQueue (command,cmdText,source_hostname,dest_hostname) values (\"%s\",\"%s\",\"%s\",\"%s\");",cmd,cmdTxt,"Tomoya",deviceName);
			/*  Always free your pointers before they go out of scope */
			sql_exec_quary(query);
			g_free(query);
		}
	g_free(deviceName);
	#endif
}
