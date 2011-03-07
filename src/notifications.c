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

#include	"sql.h"
#include	"notifications.h" 
#ifndef _WIN32
	#include 	<dbus/dbus.h>
	#include	<dbus/dbus-glib.h>
#endif
#include	<glib.h>
#include	<glib/gprintf.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#ifdef _WIN32
	#include	<winsock.h>
	#include	<mysql.h>
	#include	<windows.h>
#else
	#include	<mysql/mysql.h>
	#include	<sys/utsname.h>
#endif




gboolean set_notification(char* tickerString,char* notificationTitle,char* notificationText) {
	#ifdef _SQL
	size_t nelem;
	size_t size;
	char* base;
	size=get_size();
	nelem=get_nelem();
	base = calloc(nelem,size);
	get_active_devices(base,size,nelem);
	char* msg = (char *)g_malloc(sizeof(tickerString)+sizeof(notificationTitle)+sizeof(notificationText)+5);
	g_sprintf(msg,"%s|%s|%s",tickerString,notificationTitle,notificationText);
	char* cmd = "TMSG";
	char* recipt = g_malloc(size);
	char* name = NULL;
	#ifndef _WIN32
		struct utsname uts;
		uname( &uts );
		name = g_malloc(strlen(uts.nodename)+2);
		int len = strlen(uts.nodename);
		char* p;
		p=name;
		for (int i = 0; i<len; i++){*p++ = uts.nodename[i];}
	#else
		char hn[500 + 1];
		DWORD dwLen = 500;
		GetComputerName(hn, &dwLen);
		name = g_malloc(strlen(hn)+2);
		int len = strlen(hn);
		char* p;
		p=name;
		for (int i = 0; i<len; i++){*p++=hn[i];}
		*p='\0';

	#endif
	for (int i = 0; i < nelem; i++) {
		char* elem = base+(i*size);
		strcpy(recipt,elem);
		char* query = (char *)g_malloc(sizeof(msg)+sizeof(recipt)+sizeof(cmd)+sizeof(name)+sizeof("Insert into cmdQueue (command,cmdText,source_hostname,dest_hostname) values (\"\",\"\",\"\",\"\")")+5);
		g_sprintf(query,"Insert into cmdQueue (command,cmdText,source_hostname,dest_hostname) values (\"%s\",\"%s\",\"%s\",\"%s\")",cmd,msg,name,recipt);
		sql_exec_quary(query);
		g_free(query);
	}
	g_free(recipt);
	g_free(msg);
	g_free(name);
	g_free(base);
	return TRUE;
	#else
	return FALSE;
	#endif
}
#ifndef _WIN32
	void get_torrent_info(DBusGProxy* proxy,char* torrent) 
	{
		#if VERBOSE >= 4
		g_printf("Torrent:%s",torrent);
		#endif
	}
#endif
