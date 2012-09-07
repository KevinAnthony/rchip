/*
 *
 *    rchip, Remote Controlled Home Integration Program
 *    Copyright (C) 2011-2012 <kevin.s.anthony@gmail.com>
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
#include "showlist.h"
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
void        update_song_info();
gboolean    get_next_command(gpointer);
gboolean    update_active_devices(gpointer);
char*       build_playing_info_sql_query(const playing_info_music,char*);

extern GThread      *network_thread;
extern GThread      *file_thread;
extern GThread      *gui_thread;
extern GAsyncQueue  *network_async_queue;
extern GMutex       *Hosts_lock;
extern hostname     *Hosts;
extern GMutex       *Userpath_lock;
extern char*        Userpath;
char*               glade_file = NULL;

/*The Main Program*/
int main(int argc, char** argv) {
    /* defines the tray_icon, as well as init gtk*/
    g_set_application_name(PACKAGE_NAME);
    parse_command_line_options(argc,argv);
    register_thread("Main Thread");
    if (!queue_init())
        print("queue_init FAILED",NULL,ERROR);
    print("Glade File",glade_file,DEBUG);
    g_thread_init(NULL);
    gtk_init(NULL,NULL);
    gtk_init(&argc, &argv);
    Hosts_lock = g_mutex_new();
    Userpath_lock = g_mutex_new();
    g_mutex_lock(Userpath_lock);
    Userpath = g_strdup(getenv("HOME"));
    g_mutex_unlock(Userpath_lock);

    settings_init();
    rest_init();
    if (!xml_init())
        print("xml_init FAILED",NULL,ERROR);
    init_hostname();
    /*sets the tray icon from the create_tray_icon*/
    create_tray_icon();
    playing_info_music pInfo = {"Artist","Album","Song",0,0,0};
    /* declares the playing info struct, and print if, if _DEBUG is definded at the top of msdaemon.c*/
    /*inits the dbus and get the first set of info*/
    dbus_init();
    pInfo = dbus_get_playing_info_music();
    print_playing_info_music(pInfo);
    get_active_devices(NULL);
    update_song_info();
    GError *error;
    if ( (network_thread = g_thread_create((GThreadFunc)rest_thread_handler, NULL, FALSE, &error)) == NULL){
        print("Error Creating Network Thread",error->message,ERROR);
        g_error_free(error);
    }

    if ( (file_thread = g_thread_create((GThreadFunc)file_thread_handler, NULL, FALSE, &error)) == NULL){
        print("Error Creating Network Thread",error->message,ERROR);
        g_error_free(error);
    }
    if ( (gui_thread = g_thread_create((GThreadFunc)gui_thread_handler, NULL, FALSE, &error)) == NULL){
        print("Error Creating Network Thread",error->message,ERROR);
        g_error_free(error);
    }

    g_timeout_add (1000,(GSourceFunc) get_next_command,NULL);
    g_timeout_add (300000,(GSourceFunc) update_active_devices,NULL);
    init_status_window(FALSE,glade_file);
    start_tray();
    g_free(Userpath);

    deauthenticate();
    return 0;
}

gboolean parse_command_line_options(int argc, char **argv) {
    static gboolean version = FALSE;
    GError *error;
    GOptionContext *context;
    static const GOptionEntry options []  = {
        {"version",'v',0, G_OPTION_ARG_NONE,&version,"Version Info",NULL},
        {"gui",'g',0,G_OPTION_ARG_STRING,&glade_file,"Optional Glade File",NULL},
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



void update_song_info() {
    /*if the dbus is active, do the following, else try and connect*/
    if (Hosts != NULL)
        if (dbus_is_connected(TRUE)) {
            playing_info_music pInfo = dbus_get_playing_info_music();
            print_playing_info_music(pInfo);
            hostname_node *hosts;
            g_mutex_lock(Hosts_lock);
            for_each_hostname(hosts){
                song_info_data* info = g_malloc(sizeof(song_info_data));
                info->pInfo.Artist = g_strdup(pInfo.Artist);
                info->pInfo.Album = g_strdup(pInfo.Album);
                info->pInfo.Song = g_strdup(pInfo.Song);
                info->pInfo.Duration = pInfo.Duration;
                info->pInfo.isPlaying = pInfo.isPlaying;
                info->hostname = g_strdup(hosts->hostname);
                queue_function_data* func = g_malloc(sizeof(queue_function_data));
                func->func = *set_song_info_rest;
                func->data = (gpointer)info;
                func->priority = TP_NORMAL;
                g_async_queue_push_sorted(network_async_queue,(gpointer)func,(GCompareDataFunc)sort_async_queue,NULL);
            }
            g_mutex_unlock(Hosts_lock);

            if (pInfo.isPlaying){
                if (g_strcmp0(pInfo.Artist,"")!=0){g_free(pInfo.Artist);}
                if (g_strcmp0(pInfo.Album,"") != 0){g_free(pInfo.Album);}
                if (g_strcmp0(pInfo.Song,"") != 0){g_free(pInfo.Song);}
            }

        }
}

gboolean get_next_command(gpointer data) {
    get_next_cmd();
    return TRUE;
}

gboolean update_active_devices(gpointer data){
    queue_function_data* func = g_malloc(sizeof(queue_function_data));
    func->func = *get_active_devices;
    func->priority = TP_NORMAL;
    func->data = NULL;
    g_async_queue_push_sorted(network_async_queue,(gpointer)func,(GCompareDataFunc)sort_async_queue,NULL);
    return TRUE;
}
