#include <config.h>

#include        "sql.h"
#include	"notifications.h" 
#ifndef _WIN32
	#include 	<dbus/dbus.h>
	#include        <dbus/dbus-glib.h>
#endif
#include        <glib.h>
#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>

gboolean setNotification(char* tickerString,char* notificationTitle,char* notificationText) {
	#ifdef _SQL
	char* msg = (char *)malloc(sizeof(tickerString)+sizeof(notificationTitle)+sizeof(notificationText)+5);
	char* recipt = "7327943473";
	char* cmd = "TMSG";
	char* hostname = "Tomoya";
	char* query = (char *)malloc(sizeof(msg)+sizeof(recipt)+sizeof(cmd)+sizeof(hostname)+sizeof("Insert into cmdQueue (command,cmdText,source_hostname,dest_hostname) values (\"\",\"\",\"\",\"\")")+5);
	sprintf(msg,"%s|%s|%s",tickerString,notificationTitle,notificationText);
	sprintf(query,"Insert into cmdQueue (command,cmdText,source_hostname,dest_hostname) values (\"%s\",\"%s\",\"%s\",\"%s\")",cmd,msg,hostname,recipt);
	sql_exec_quary(query);
	return TRUE;
	#else
	return FALSE;
	#endif
}
#ifndef _WIN32
	void getTorrentInfo(DBusGProxy* proxy,char* torrent) 
	{
		printf("\n\n\n\n\n\n\n\n%s\n\n\n\n\n\n\n\n\n\n\n",torrent);
	}
#endif
