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
#ifdef _WIN32
        #include        <winsock.h>
        #include        <mysql.h>
        #include        <windows.h>
#else
        #include        <mysql/mysql.h>
        #include        <sys/utsname.h>
#endif




gboolean setNotification(char* tickerString,char* notificationTitle,char* notificationText) {
	#ifdef _SQL
	size_t nelem;
	size_t size;
	char* base;
	size=get_size();
        nelem=get_nelem();
        base = calloc(nelem,size);
        get_active_devices(base,size,nelem);
	char* msg = (char *)malloc(sizeof(tickerString)+sizeof(notificationTitle)+sizeof(notificationText)+5);
	sprintf(msg,"%s|%s|%s",tickerString,notificationTitle,notificationText);
	char* cmd = "TMSG";
	char* recipt = malloc(size);
        char* name = NULL;
        #ifndef _WIN32
                struct utsname uts;
                uname( &uts );
                name = malloc(strlen(uts.nodename)+2);
                int len = strlen(uts.nodename);
                char* p;
                p=name;
                for (int i = 0; i<len; i++){*p++ = uts.nodename[i];}
        #else
                char hn[500 + 1];
                DWORD dwLen = 500;
                GetComputerName(hn, &dwLen);
                name = malloc(strlen(hn)+2);
                int len = strlen(hn);
                char* p;
                p=name;
                for (int i = 0; i<len; i++){*p++=hn[i];}
                *p='\0';

        #endif
	for (int i = 0; i < nelem; i++) {
        	char* elem = base+(i*size);
                strcpy(recipt,elem);
        	char* query = (char *)malloc(sizeof(msg)+sizeof(recipt)+sizeof(cmd)+sizeof(name)+sizeof("Insert into cmdQueue (command,cmdText,source_hostname,dest_hostname) values (\"\",\"\",\"\",\"\")")+5);
	        sprintf(query,"Insert into cmdQueue (command,cmdText,source_hostname,dest_hostname) values (\"%s\",\"%s\",\"%s\",\"%s\")",cmd,msg,name,recipt);
        	sql_exec_quary(query);
		free(query);
	}
	free(recipt);
	free(msg);
	free(name);
	free(base);
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
