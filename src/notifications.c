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
	char* query = (char *)malloc(1024);
	char* msg = (char *)malloc(256);
	sprintf(msg,"%s|%s|%s",tickerString,notificationTitle,notificationText);
	char* recipt = "7327943473";
	//char*  recipt = (char *)malloc(64);
	//recipt = get_registered_devices_message();
	sprintf(query,"Insert into cmdQueue (command,cmdText,source_hostname,dest_hostname) values (\"%s\",\"%s\",\"%s\",\"%s\")","TMSG",msg,"Tomoya",recipt);
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
