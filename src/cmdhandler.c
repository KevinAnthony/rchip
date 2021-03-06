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

#include <glib.h>
#include <glib/gprintf.h>
#include <stdlib.h>
#include <string.h>
/*for hostname on unix and unix-like systems*/
#include <sys/utsname.h>
#ifndef DBUSOFF
/*windows doesn't have a dbus so we can't use it*/
#include "dbus.h"
#endif
#include "cmdhandler.h"
#include "settings.h"
#include "status.h"
#include "rest.h"
#include "utils.h"
#include "xml.h"

/*
 * (insert mad scientist type laughter here)
 */

extern GAsyncQueue  *network_async_queue;
extern GAsyncQueue  *gui_async_queue;
extern GMutex       *Hosts_lock;
extern hostname     *Hosts;

void get_next_cmd() {
    /*uts.nodename is the hostname of the computer*/
    struct utsname uts;
    uname( &uts );
    queue_function_data* func = g_malloc(sizeof(queue_function_data));
    func->func = *get_cmd_from_server;
    func->priority = TP_NORMAL;
    func->data = (gpointer)g_strdup(uts.nodename);
    g_async_queue_push_sorted(network_async_queue,(gpointer)func,(GCompareDataFunc)sort_async_queue,NULL);
}

gboolean process_cmd(char* cmd,char* cmdTxt) {
    if ((cmd != NULL) && (g_strcmp0(cmd,"") != 0)){
        gboolean xmli = xml_init();
        gboolean xmlf = xml_find_command(cmd);
        if (!xmli){
            print("XML_INIT PROBLEM",NULL,ERROR);
            return FALSE;
        }
        if (!xmlf){
            print("XML_FIND_COMMAND PROBLEM",NULL,ERROR);
            print(cmd,cmdTxt,ERROR);
            return FALSE;
        }
        if (xmli && xmlf){
            char* type = xml_get_type();
            char* musicOrVideo = xml_get_music_or_video();
            if(g_strcmp0(type,"SYSTEM") == 0){
                if (g_strcmp0(cmdTxt,"") != 0){
                    char* temp_cmdTxt = g_strescape(cmdTxt,"");
                    g_free(cmdTxt);
                    cmdTxt = replace_str(temp_cmdTxt,"(","\\(");
                    g_free(temp_cmdTxt);
                    temp_cmdTxt = replace_str(cmdTxt,")","\\)");
                    g_free(cmdTxt);
                    cmdTxt = replace_str(temp_cmdTxt,"\'","\\\'");
                    g_free(temp_cmdTxt);
                    temp_cmdTxt = replace_str(cmdTxt,";","\\;");
                    g_free(cmdTxt);
                    cmdTxt = g_strdup(temp_cmdTxt);
                    g_free(temp_cmdTxt);
                }
                gchar* command = g_strdup_printf ("%s %s&",xml_get_system_command(),cmdTxt);
                system(command);
                g_free(command);
            } else if (g_strcmp0(type,"DBUS") == 0) {
                char* argument = xml_get_dbus_argument();
                char* argument_str = xml_get_dbus_argument_type();
                char* dbus_command = xml_get_dbus_command();
                if (argument != NULL){
                    if(g_strcmp0(musicOrVideo,"MUSIC") == 0){
                        send_command_to_music_player_with_argument(dbus_command,argument_str,argument);
                    } else {
                        send_command_to_video_player_with_argument(dbus_command,argument_str,argument);
                    }
                    g_free(argument);
                    g_free(argument_str);
                } else {
                    if(g_strcmp0(musicOrVideo,"MUSIC") == 0){
                        send_command_to_music_player(dbus_command);
                    } else {
                        send_command_to_video_player(dbus_command);
                    }
                }
                g_free(dbus_command);
            } else if (g_strcmp0(type,"DBUS_MPRIS") == 0){
                    send_command_to_mpris(cmd,cmdTxt,musicOrVideo);
            }
            g_free(type);
            return TRUE;
        } else {
            return FALSE;
        }
    }
    return TRUE;
}

/* insert command and command text into cmdQueue, once per hsotname*/
void send_cmd(char* cmd, char* cmdTxt, thread_priority priority) {
    print(cmd,cmdTxt,DEBUG);
    hostname_node *hosts;
    g_mutex_lock(Hosts_lock);
    for_each_hostname(hosts){
        queue_function_data* func = g_malloc(sizeof(queue_function_data));
        command_data* data = g_malloc(sizeof(command_data));
        data->command = g_strdup(cmd);
        data->command_text = g_strdup(cmdTxt);
        data->hostname = g_strdup(hosts->hostname);
        func->func = *send_cmd_to_server;
        func->priority = priority;
        func->data = (gpointer)data;
        g_async_queue_push_sorted(network_async_queue,(gpointer)func,(GCompareDataFunc)sort_async_queue,NULL);
    }
    g_mutex_unlock(Hosts_lock);
}
