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
#include "dbus.h"
#ifdef _JSON
#include "rest.h"
#include "cmdhandler.h"
#endif
#include "utils.h"
#include "xml.h"

void        print_version();
gboolean    parse_command_line_options(int,char**);
gboolean    update_song_info(gpointer);
gboolean    get_next_command(gpointer);
gboolean    update_active_devices(gpointer);
char*       build_playing_info_sql_query(const struct playing_info_music,char*);

/*The Main Program*/
int main(int argc, char** argv) {
    /* defines the tray_icon, as well as init gtk*/
    g_set_application_name(PACKAGE_NAME);
    GtkStatusIcon *tray_icon;
    parse_command_line_options(argc,argv);
    gtk_init(&argc, &argv);
    settings_init();
    authenticate();
    if (!xml_init()){
        g_error("xml_init FAILED\n");
    }
    init_hostname();
    /*sets the tray icon from the create_tray_icon*/
    tray_icon = create_tray_icon();
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
    get_active_devices();
    g_timeout_add (1000,(GSourceFunc) get_next_command,NULL);
    g_timeout_add (10000,(GSourceFunc) update_song_info,NULL);
    g_timeout_add (300000,(GSourceFunc) update_active_devices,NULL);
    init_status_window(FALSE);
    gtk_widget_show(tray_icon);
    start_tray();
    deauthenticate();
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
        g_printf("\n%s %s\n\nCopyright (C) 2010-%i NoSide Racing, llc.\nLicense GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\nThis is free software: you are free to change and redistribute it.\nThere is NO WARRANTY, to the extent permitted by law.\n\nWritten By %s\n",PACKAGE_NAME,PACKAGE_VERSION,COMPILE_YEAR,PROGRAMMERS_NAME);
}



gboolean update_song_info(gpointer data) {
    /*if the dbus is active, do the following, else try and connect*/
    if (dbus_is_connected(TRUE)) {
        struct playing_info_music pInfo = dbus_get_playing_info_music();
        #if VERBOSE >= 4
        print_playing_info_music(pInfo);
        #endif
        hostname_node *hosts;
        for_each_hostname(hosts){
            set_song_info_rest(pInfo,hosts->hostname);
        }
        if (pInfo.isPlaying){
            if (g_strcmp0(pInfo.Artist,"")!=0){g_free(pInfo.Artist);}
            if (g_strcmp0(pInfo.Album,"") != 0){g_free(pInfo.Album);}
            if (g_strcmp0(pInfo.Song,"") != 0){g_free(pInfo.Song);}
        }
    }
    return TRUE;
}
gboolean get_next_command(gpointer data) {
    get_next_cmd();
    return TRUE;
}
gboolean update_active_devices(gpointer data){
        get_active_devices();    
        return TRUE;
}
