#include <stdlib.h>
#include <stdio.h>
#include "mstray.h"
#include "dbus.h"
#include "sql.h"
#include "cmdhandler.h"

gboolean daemon_loop(gpointer);
gboolean update_active_devices(gpointer);
char* build_playing_info_sql_query(const struct playing_info,char*);

size_t nelem;
size_t size; 
char* base;

/*The Main Program*/
int main(int argc, char** argv) {
	/* defines the tray_icon, as well as init gtk*/
	GtkStatusIcon *tray_icon;
	gtk_init(&argc, &argv);
	sql_init();
	size=get_size();
	nelem=get_nelem();
	base = calloc(nelem,size);
	get_active_devices(base,size,nelem);
	/*sets the tray icon from the create_tray_icon*/
	tray_icon = create_tray_icon();
	/* declares the playing info struct, and print if, if _DEBUG is definded at the top of msdaemon.c*/
	struct playing_info pInfo = {"Artist","Album","Song",0,3600,0};
	#ifdef _DEBUG
		print_playing_info(pInfo);
	#endif
	/*inits the dbus and get the first set of info*/
	dbus_init();
	pInfo = dbus_get_playing_info();
	#ifdef _DEBUG
		print_playing_info(pInfo);
	#endif
	/*adds the fuction daemon_loop to the gtk main loop, and executes it evert 1/2 second*/
	char* hostname = malloc(size);
                int i = 0;
                for (i = 0; i < nelem; i++) {
                        char* elem = base+(i*size);
                        strcpy(hostname,elem);
                }
	g_timeout_add (500,(GSourceFunc) daemon_loop,NULL);
	g_timeout_add (5000,(GSourceFunc) update_active_devices,NULL);
	start_tray();
	free(base);
	return 0;
}

gboolean daemon_loop(gpointer data) {
	/*if the dbus is active, do the following, else try and connect*/
	get_next_cmd();
	if (dbus_is_connected(TRUE)) {
		struct playing_info pInfo = dbus_get_playing_info();
		#ifdef _DEBUG
			print_playing_info(pInfo);
		#endif
		char* hostname = malloc(size);
		int i = 0;
		for (i = 0; i < nelem; i++) {
			char* elem = base+(i*size);
			strcpy(hostname,elem);
			char* query = build_playing_info_sql_query(pInfo,hostname);
			sql_exec_quary(query);
		}
			
		free(hostname);	
	}
	return TRUE;
}

gboolean update_active_devices(gpointer data){
	size=get_size();
        nelem=get_nelem();
	get_active_devices(base,size,nelem);
	return TRUE;
}

char* build_playing_info_sql_query(const struct playing_info pInfo,char* hostname) {
	char* query =(char *)malloc(1024);
	sprintf(query,"INSERT INTO rymBoxInfo (artist,album,title,etime,tottime,isplaying,dest_hostname) VALUES (\"%s\",\"%s\",\"%s\",\"%i\",\"%i\",\"%i\",\"%s\") ON DUPLICATE KEY UPDATE artist=\"%s\",album=\"%s\",title=\"%s\",etime=\"%i\",tottime=\"%i\",isplaying=\"%i\",dest_hostname=\"%s\";",pInfo.Artist,pInfo.Album,pInfo.Song,pInfo.Elapised_time,pInfo.Duration,pInfo.isPlaying,hostname,pInfo.Artist,pInfo.Album,pInfo.Song,pInfo.Elapised_time,pInfo.Duration,pInfo.isPlaying,hostname);
	return query;
}

