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

/* returns the next hostname, from a pointer based array */
char* get_next_host(void* base,size_t size, size_t next){
	return base+(next*size);
}

/*
 * get the next command from database and handle it's exececution
 * TODO:
 * I'm playing with the idea of of switching to an XML based command system
 * looking like this
 * <command>
 *   <name>MUSICPLAY</name>
 *   <type>SYSTEM|DBUS</type>
 *   <dbus_command>org.banshee.banshee.PlayerEngine.play</dbus_command>
 *   <system_command>smplayer --play</system_command>
 *   <argument_type>IN_G_VARIANT_FORMAT or nothing</argument_type>
 *   <argument>argument</argument>
 *   <commant>Something not included in the command</command>
 * </command>
 * and then find in xml where cmd=name
 * if type = dbus
 * execute proxy(dbus_command,argument_type, argument)
 * or proxy(dbus_command)
 * if type = system
 * execute(%s%s, system_command, argument)
 * reducing the code by much, maybe even manymuch, line
 * 
 * (insert mad scientist type laughter here)
 */
void get_next_cmd() {
	char* cmd = "";
	char* cmdTxt = "";
	char* source = "";
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
	#if VERBOSE >= 4
		g_print("ID:%i\ncmd:%s\ncmdTxt:%s\n",cmdID,cmd,cmdTxt);
	#endif
	/*command has to be valid*/
	if (!(strcmp(cmd," "))){
		return;
	}
	/* make sure it's a valid command*/
	if (cmdID != 0){
		/* process the command */
		if (!(strcmp(cmd,"STRB"))) {
			#if VERBOSE >= 1
			g_warning("Warning:STRB is deprecated");
			#endif
			#ifdef _SQL
			delete_from_cmdQueue(cmdID);
			#endif
		} else if (!(strcmp(cmd,"SPRB"))) {
			#if VERBOSE >= 1
			g_warning("Warning:SPRB is deprecated");
			#endif
			#ifdef _SQL
			delete_from_cmdQueue(cmdID);
			#endif
		} else if (!(strcmp(cmd,"NEXTRB"))) {
			#ifndef _WIN32
			#ifndef DBUSOFF
			#ifdef RHYTHMBOX
			if(send_command_to_music_player("next")){
			#endif
			#ifdef BANSHEE
			if(send_command_to_music_player_with_argument("Next","(b)","TRUE")){
			#endif
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			} else {
				#if VERBOSE >= 2
				g_error("Could not Next\n");
				#endif
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			}
			#else
			#ifdef _SQL
			delete_from_cmdQueue(cmdID);
			#endif
			#endif
			#else
			#ifdef _SQL
			delete_from_cmdQueue(cmdID);
			#endif
			#endif
		} else if (!(strcmp(cmd,"BACKRB"))) {
			#ifndef _WIN32
			#ifndef DBUSOFF
			#ifdef RHYTHMBOX
			if(send_command_to_music_player("previous")) {
			#endif
			#ifdef BANSHEE
			if(send_command_to_music_player_with_argument("Previous","(b)","TRUE")) {
			#endif
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			} else {
				#if VERBOSE >= 2
				g_error("Could not BACK\n");
				#endif
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			}
			#else
			#ifdef _SQL
			delete_from_cmdQueue(cmdID);
			#endif
			#endif
		#else
			#ifdef _SQL
			delete_from_cmdQueue(cmdID);
			#endif
		#endif
		} else if (!(strcmp(cmd,"PLAYRB"))) {
		#ifndef _WIN32
			#ifndef DEBUS
			#ifdef RHYTHMBOX
			if(send_command_to_music_player_with_argument("playPause","(b)","TRUE")) {
			#endif
			#ifdef BANSHEE
			if(send_command_to_music_player("Play")){
			#endif
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			} else {
				#if VERBOSE >= 2
				g_error("Could not PLAY\n");
				#endif
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			}
			#else
			#ifdef _SQL
			delete_from_cmdQueue(cmdID);
			#endif
			#endif
			#else
			#ifdef _SQL
			delete_from_cmdQueue(cmdID);
			#endif
			#endif
		} else if (!(strcmp(cmd,"STOPRB"))) {
			#ifndef _WIN32
			#ifndef DBUSOFF
			#ifdef RHYTHMBOX
			if(send_command_to_music_player_with_argument("playPause","(b)","TRUE")) {
			#endif
			#ifdef BANSHEE
			if(send_command_to_music_player("Pause")){
			#endif
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			} else {
				#if VERBOSE >= 2
				g_error("Could not STOP\n");
				#endif
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			}
			#else
			#ifdef _SQL
			delete_from_cmdQueue(cmdID);
			#endif
			#endif

			#else
			#ifdef _SQL
			delete_fvirom_cmdQueue(cmdID);
			#endif
			#endif
		} else if (!(strcmp(cmd,"PLAYSM"))) {
			if (system("smplayer -send-action play")) {
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			} else {
				#if VERBOSE >= 2
				g_warning("Could not Play SMplayer\n");
				#endif
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			}

		} else if (!(strcmp(cmd,"STOPSM"))) {
			if (system("smplayer -send-action stop")) {
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			} else {
				#if VERBOSE >= 2
				g_warning("Could not Stop SMplayer\n");
				#endif
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			}
		} else if (!(strcmp(cmd,"PAUSESM"))) {
			if (system("smplayer -send-action pause")) {
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);	
				#endif
			} else {
				#if VERBOSE >= 2
				g_warning("Could not Pause SMplayer\n");
				#endif
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			}

		} else if (!(strcmp(cmd,"SKIPFSM"))) {
			if (system("smplayer -send-action forward1")) {
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			} else {
				#if VERBOSE >= 2
				g_warning("Could not Skip Foward SMplayer\n");
				#endif
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			}
		} else if (!(strcmp(cmd,"SKIPBSM"))) {
			if (system("smplayer -send-action rewind1")) {
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			} else {
				#if VERBOSE >= 2
				g_warning("Could not Skip Backwards SMplayer\n");
				#endif
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			}
		} else if (!(strcmp(cmd,"FULLONSM"))) {
			if (system("smplayer -send-action fullscreen")) {
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			} else {
				#if VERBOSE >= 2
				g_warning("Could not Fullscreen SMplayer\n");
				#endif
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			}
		} else if (!(strcmp(cmd,"FULLOFFSM"))) {
			if (system("smplayer -send-action exit-fullscreen")) {
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			} else {
				#if VERBOSE >= 2
				g_warning("Could not Fullscreen off SMplayer\n");
				#endif
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			}
		} else if (!(strcmp(cmd,"OPENSM"))) {
			
			char* command;
			#ifdef _WIN32
			char* filename = cmdTxt;
			char* cmd= "smplayer.exe";
			char* path="c:\\Program Files\\smplayer\\smplayer.exe";
			//command = malloc(1);
			while (*filename++ != '\0') {if (*filename == '/') { *filename = '\\';} }
			if(spawnl(P_NOWAIT, path ,cmd ,cmdTxt,NULL) >= 0) {
			#else
			char* text = "smplayer ";
			command = g_malloc(strlen(cmdTxt)+strlen(text)+1);
			g_sprintf(command,"%s%s&",text,cmdTxt);
			#endif
			if (system(command)) {
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			} else {
				#if VERBOSE >= 2
				g_warning("Could not open SMplayer\n");
				#endif
				#ifdef _WIN32
				#if VERBOSE >= 2
				g_printf("Last error: %i\n",myerrno);
				#endif
				#endif
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			}
			g_free(command);	
		} else if (!(strcmp(cmd,"MUTESM"))) {
			if (system("smplayer -send-action mute")) {
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			} else {
				#if VERBOSE >= 2
				g_warning("Could not Play SMplayer\n");
				#endif
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			}
		} else if (!(strcmp(cmd,"QUITSM"))) {
			if (system("smplayer -send-action quit")) {
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			} else {
				#if VERBOSE >= 2
				g_warning("Could not Play SMplayer\n");
				#endif
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			}
		} else {
			#if VERBOSE >= 2
			g_printf("Command Not Recognized: ::%s::\n",cmd);
			#endif
			#ifdef _SQL
			delete_from_cmdQueue(cmdID);
			#endif
		}
		g_free(cmd);
		g_free(cmdTxt);
		g_free(source);
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
			 /* TODO:replace this and g_sprintf with self allocating memory commands :) */
			strcpy(deviceName,elem);    
			char* query = (char *)g_malloc(1024);
			g_sprintf(query,"INSERT INTO cmdQueue (command,cmdText,source_hostname,dest_hostname) values (\"%s\",\"%s\",\"%s\",\"%s\");",cmd,cmdTxt,"Tomoya",deviceName);
			/*  Always free your pointers before they go out of scope */
			sql_exec_quary(query);
			g_free(query);
		}
	g_free(deviceName);
	#endif
}
