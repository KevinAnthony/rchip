#include <config.h>
#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include "tray.h"
#include "settings.h"
#ifndef _WIN32
	#include "dbus.h"
#endif
#ifndef _NOSQL
#include "sql.h"
#include "cmdhandler.h"
#endif

gboolean daemon_loop(gpointer);
gboolean update_active_devices(gpointer);
#ifndef _WIN32
	char* build_playing_info_sql_query(const struct playing_info_music,char*);
#endif
size_t nelem;
size_t size; 
char* base;

/*The Main Program*/
int main(int argc, char** argv) {
	/* defines the tray_icon, as well as init gtk*/
	GtkStatusIcon *tray_icon;
	gtk_init(&argc, &argv);
	g_set_application_name("rchip");
	#ifndef _NOSQL
		sql_init();
		update_daemon_sql();
		size=get_size();
		nelem=get_nelem();
		base = calloc(nelem,size);
		get_active_devices(base,size,nelem);
	#endif
	/*sets the tray icon from the create_tray_icon*/
	tray_icon = create_tray_icon();
	#ifndef _NOSQL
	#ifndef _WIN32
		settings_init();	
		struct playing_info_music pInfo = {"Artist","Album","Song",0,0,0};	
		/* declares the playing info struct, and print if, if _DEBUG is definded at the top of msdaemon.c*/
		#if VERBOSE >= 4
		print_playing_info_music(pInfo);
		#endif
		/*inits the dbus and get the first set of info*/
		dbus_init();

		pInfo = dbus_get_playing_info_music();
		#if VERBOSE >= 4
		print_playing_info_music(pInfo);
		#endif
	#endif
	g_timeout_add (500,(GSourceFunc) daemon_loop,NULL);
	g_timeout_add (5000,(GSourceFunc) update_active_devices,NULL);
	#endif
	start_tray();
	#ifndef _NOSQL
		free(base);
	#endif
	return 0;
}
#ifndef _NOSQL
#ifndef _WIN32
	gboolean daemon_loop(gpointer data) {
		/*if the dbus is active, do the following, else try and connect*/
		get_next_cmd();
		if (dbus_is_connected(TRUE)) {
			struct playing_info_music pInfo = dbus_get_playing_info_music();
			#if VERBOSE >= 4
			print_playing_info_music(pInfo);
			#endif
			char* hostname = malloc(size);
			for (int i = 0; i < nelem; i++) {
				char* elem = base+(i*size);
				strcpy(hostname,elem);
				char* query = build_playing_info_sql_query(pInfo,hostname);
				sql_exec_quary(query);
				free(query);
			}
				
			free(hostname);	
		}
		return TRUE;
	}
#else
	gboolean daemon_loop(gpointer data){
		get_next_cmd();
		return TRUE;
	}
#endif

gboolean update_active_devices(gpointer data){
	size=get_size();
        nelem=get_nelem();
	get_active_devices(base,size,nelem);
	return TRUE;
}
#ifndef _WIN32
	char* build_playing_info_sql_query(const struct playing_info_music pInfo,char* hostname) {
		char* query =(char *)malloc(1024);
		sprintf(query,"INSERT INTO rymBoxInfo (artist,album,title,etime,tottime,isplaying,dest_hostname) VALUES (\"%s\",\"%s\",\"%s\",\"%i\",\"%i\",\"%i\",\"%s\") ON DUPLICATE KEY UPDATE artist=\"%s\",album=\"%s\",title=\"%s\",etime=\"%i\",tottime=\"%i\",isplaying=\"%i\",dest_hostname=\"%s\";",pInfo.Artist,pInfo.Album,pInfo.Song,pInfo.Elapised_time,pInfo.Duration,pInfo.isPlaying,hostname,pInfo.Artist,pInfo.Album,pInfo.Song,pInfo.Elapised_time,pInfo.Duration,pInfo.isPlaying,hostname);
		return query;
	}
#endif
#endif
