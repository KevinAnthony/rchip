/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
			ihar* hostname = g_malloc(size);
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

#include <stdlib.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <string.h>
#include "tray.h"
#include "status.h"
#include "settings.h"
#ifndef _WIN32
	#include "dbus.h"
#endif
#ifndef _NOSQL
#include "sql.h"
#include "cmdhandler.h"
#endif
#include "xml.h"

void print_version();
gboolean parse_command_line_options(int,char**);
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
	g_set_application_name(PACKAGE_NAME);
	GtkStatusIcon *tray_icon;
	parse_command_line_options(argc,argv);
	gtk_init(&argc, &argv);
	if (!xml_init()){
		g_error("xml_init FAILED\n");
	}
	#ifndef _WIN32
		settings_init();
	#endif
	#ifndef _NOSQL
		sql_init();
		update_daemon_sql();
		size=get_size();
		nelem=get_nelem();
		base = g_malloc0(nelem*size);
		get_active_devices(base,size,nelem);
	#endif
	/*sets the tray icon from the create_tray_icon*/
	tray_icon = create_tray_icon();
	#ifndef _NOSQL
	#ifndef _WIN32
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
	init_status_window(FALSE);
	start_tray();
	#ifndef _NOSQL
		g_free(base);
	#endif
	return 0;
}

gboolean parse_command_line_options(int argc, char **argv) {
        static gboolean version = FALSE;
	GError *error;
        GOptionContext *context;
        static const GOptionEntry options []  = {
        {"version",'v',0, G_OPTION_ARG_NONE,&version,("Version Info"),NULL},
        {NULL}
        };
        context = g_option_context_new (NULL);
        g_option_context_add_main_entries (context, options, NULL);
        if (g_option_context_parse (context, &argc, &argv, &error) == FALSE) {
                g_printf ("%s\nRun '%s --help' to see a full list of available command line options.\n",
                         error->message, argv[0]);
                g_error_free (error);
                g_option_context_free (context);
                return FALSE;
        }
        g_option_context_free (context);
        if (version){
                print_version();
                exit(0);
        } 
        return TRUE;
}
void print_version(){
        g_printf("\n%s %s\n\nCopyright (C) %i NoSide Racing, llc.\nLicense GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\nThis is free software: you are free to change and redistribute it.\nThere is NO WARRANTY, to the extent permitted by law.\n\nWritten By %s\n",PACKAGE_NAME,PACKAGE_VERSION,COMPILE_YEAR,PROGRAMMERS_NAME);
}



#ifndef _NOSQL
#ifndef _WIN32
	gboolean daemon_loop(gpointer data) {
		size=get_size();
	        nelem=get_nelem();
		/*if the dbus is active, do the following, else try and connect*/
		get_next_cmd();
		if (dbus_is_connected(TRUE)) {
			struct playing_info_music pInfo = dbus_get_playing_info_music();
			#if VERBOSE >= 4
			print_playing_info_music(pInfo);
			#endif
			if (base != NULL){
				for (int i = 0; i < nelem; i++) {
					char* elem = base+(i*size);
					printf("i = %i\n",i);
					char* hostname = g_strndup(elem,size);
					char* query = build_playing_info_sql_query(pInfo,hostname);
					sql_exec_quary(query);
					g_free(query);
					g_free(hostname);
				}
			} else {
				g_warning("base is null main.c");
			}
			if (pInfo.isPlaying){
				if (g_strcmp0(pInfo.Artist,"")!=0){g_free(pInfo.Artist);}
				if (g_strcmp0(pInfo.Album,"") != 0){g_free(pInfo.Album);}
				if (g_strcmp0(pInfo.Song,"") != 0){g_free(pInfo.Song);}
			}
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
		if ((pInfo.Artist != NULL) || (g_strcmp0(pInfo.Artist,"Artist") != 0)){

			char* query =g_strdup_printf("INSERT INTO rymBoxInfo (artist,album,title,etime,tottime,isplaying,dest_hostname) VALUES (\"%s\",\"%s\",\"%s\",\"%i\",\"%i\",\"%i\",\"%s\") ON DUPLICATE KEY UPDATE artist=\"%s\",album=\"%s\",title=\"%s\",etime=\"%i\",tottime=\"%i\",isplaying=\"%i\",dest_hostname=\"%s\";",pInfo.Artist,pInfo.Album,pInfo.Song,pInfo.Elapised_time,pInfo.Duration,pInfo.isPlaying,hostname,pInfo.Artist,pInfo.Album,pInfo.Song,pInfo.Elapised_time,pInfo.Duration,pInfo.isPlaying,hostname);
			return query;
		} else {
			return "Select NULL;";
		}
	}
#endif
#endif
