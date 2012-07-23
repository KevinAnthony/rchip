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

#ifdef _JSON

#include    <curl/curl.h>
#include    <json/json.h>
#include    <glib.h>
#include    <glib/gprintf.h>
#include    <string.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    "rest.h"
#include    "settings.h"
#include    "utils.h"
#include    "cmdhandler.h"
#include    <sys/utsname.h>

extern gboolean     program_active;
extern GAsyncQueue  *network_async_queue;
extern GMutex       *Hosts_lock;
extern hostname     *Hosts;

//TODO: This should be a setting, not hard coded
char* URL = "http://www.nosideholdings.com/json";
CURL *session;

void rest_init() {
    authenticate();
    update_daemon_server();
}

void rest_thread_handler(gpointer* NotUsed){
    while (program_active){
        gpointer *data = g_async_queue_try_pop (network_async_queue);
        if (data){
            queue_function_data *function_data = (queue_function_data*) data;
            function_data->func(function_data->data);
            g_free(function_data);
        }
        if (g_async_queue_length(network_async_queue) == 0)
            usleep(SLEEP_TIME);

    }
}

gpointer* get_cmd_from_server(gpointer *data) {
    char* hostname = (char*) data;
    if (session) {
        char* url = g_strconcat(URL,"/getcommand/?host=",curl_easy_escape(session,hostname,strlen(hostname)),NULL);
        curl_easy_setopt(session, CURLOPT_URL, url);
        curl_easy_setopt(session, CURLOPT_COOKIEFILE,"~/.cache/rchipcookies");
        curl_easy_setopt(session, CURLOPT_WRITEFUNCTION, get_commands_callback);
        curl_easy_perform(session);
        g_free(url);
    }
    g_free(data);
    return NULL;
}

size_t get_commands_callback(void *ptr,size_t size, size_t count, void* stream){
    long http_code = 0;
    curl_easy_getinfo (session, CURLINFO_RESPONSE_CODE, &http_code);
    if (http_code == 200) {
        json_object *jarray = json_tokener_parse((char*)ptr);
        json_object *json_success = json_object_object_get(jarray, "success");
        if (g_strcmp0(json_object_to_json_string(json_success),"true") == 0){
            json_object *json_data = json_object_object_get(jarray, "data");
            for (int i = 0; i< json_object_array_length(json_data); i++){
                char *cmd;
                char *cmd_txt;
                json_object_object_foreach(json_object_array_get_idx(json_data, i), key, val){
                    if (g_strcmp0(key,"command") == 0){
                        cmd = json_object_get_string(val);
                    }
                    if (g_strcmp0(key,"command_text") == 0){
                        cmd_txt = json_object_get_string(val);
                    }
                }
                process_cmd(cmd,cmd_txt);
            }
        }
    }
    return 0;
}

gpointer* send_cmd_to_server(gpointer* data){
    command_data* command = (command_data*) data;
    printf("Command going to server = %s  %s\n",command->command,command->command_text);
    struct utsname uts;
    uname( &uts );
    if (session) {
        char* url = g_strconcat(URL,"/sendcommand/?command=",curl_easy_escape(session,command->command,strlen(command->command)),"&command_text=",curl_easy_escape(session,command->command_text,strlen(command->command_text)),"&source_hostname=",curl_easy_escape(session,uts.nodename,strlen(uts.nodename)),"&destination_hostname=",curl_easy_escape(session,command->hostname,strlen(command->hostname)),NULL);
        curl_easy_setopt(session, CURLOPT_URL, url);
        curl_easy_setopt(session, CURLOPT_COOKIEFILE,"~/.cache/rchipcookies");
        curl_easy_perform(session);
    }
    g_free(command->command);
    g_free(command->command_text);
    g_free(command->hostname);
    g_free(data);
    return NULL;
}

char* get_registered_devices_message(){
    return NULL;
}

gpointer* get_active_devices( gpointer* data ){
    if (session) {
        char* url = g_strconcat(URL,"/getremotedevice/",NULL);
        curl_easy_setopt(session, CURLOPT_URL, url);
        curl_easy_setopt(session, CURLOPT_COOKIEFILE,"~/.cache/rchipcookies");
        curl_easy_setopt(session, CURLOPT_WRITEFUNCTION, get_active_decives_callback);
        curl_easy_perform(session);
        g_free(url);
    }
    return NULL;
}

size_t get_active_decives_callback(void *ptr,size_t size, size_t count, void* stream){
    json_object *jarray = json_tokener_parse((char*)ptr);
    json_object *json_success = json_object_object_get(jarray, "success");
    if (g_strcmp0(json_object_to_json_string(json_success),"true") == 0){
        json_object *json_data = json_object_object_get(jarray, "data");
        g_mutex_lock(Hosts_lock);
        for (int i = 0; i< json_object_array_length(json_data); i++){
            json_object_object_foreach(json_object_array_get_idx(json_data, i), key, val){
                char* host = json_object_get_string(val);
                Hosts->add(host);
            }
        }
        g_mutex_unlock(Hosts_lock);
    }
    return 0;
}
void update_daemon_server(){
    if (session){
        struct utsname uts;
        uname( &uts );
        char* video_path = get_setting_str(VIDEO_ROOT);
        char* url = g_strconcat(URL,"/registerdaemon/?hostname=",curl_easy_escape(session,uts.nodename,strlen(uts.nodename)),"&video_path=",curl_easy_escape(session,video_path,strlen(video_path)),NULL);

        curl_easy_setopt(session, CURLOPT_URL, url);
        curl_easy_setopt(session, CURLOPT_COOKIEFILE,"~/.cache/rchipcookies");
        curl_easy_setopt(session, CURLOPT_WRITEFUNCTION, get_active_decives_callback);
        curl_easy_perform(session);
        g_free(url);
    }
}

gpointer* set_song_info_rest(gpointer *data) {
    song_info_data* info = (song_info_data*) data;
    if (info->pInfo.isPlaying != 0){
        if (session) {
            char* url = g_strdup_printf("%s/setsonginfo/?artist=%s&album=%s&song=%s&elapsed_time=%d&total_time=%d&is_playing=%d&dest_hostname=%s",
                    URL,curl_easy_escape(session,info->pInfo.Artist,strlen(info->pInfo.Artist)),
                    curl_easy_escape(session,info->pInfo.Album,strlen(info->pInfo.Album)),
                    curl_easy_escape(session,info->pInfo.Song,strlen(info->pInfo.Song)),
                    info->pInfo.Elapised_time,info->pInfo.Duration,info->pInfo.isPlaying,
                    curl_easy_escape(session,info->hostname,strlen(info->hostname)));
            curl_easy_setopt(session, CURLOPT_URL, url);
            curl_easy_setopt(session, CURLOPT_COOKIEFILE,"~/.cache/rchipcookies");
            curl_easy_perform(session);
            g_free(info->pInfo.Artist);
            g_free(info->pInfo.Album);
            g_free(info->pInfo.Song);
            g_free(url);
        }
    }
    g_free(info->hostname);
    g_free(data);
    return NULL;
}

void authenticate( void ){
    char* username = get_setting_str(REST_USERNAME);
    char* password = get_setting_str(REST_PASSWORD);
    char* url = g_strconcat(URL,"/authenticate/?username=",username,"&password=",password,NULL);
    session = curl_easy_init();
    if (session) {
        curl_easy_setopt(session, CURLOPT_URL, url);
        curl_easy_setopt(session, CURLOPT_COOKIEFILE,"~/.cache/rchipcookies");
        curl_easy_perform(session);
    }
    g_free(url);
}

void deauthenticate( void ){
    char* url = g_strconcat(URL,"/deauthenticate",NULL);
    if (session) {
        curl_easy_setopt(session, CURLOPT_URL, url);
        curl_easy_setopt(session, CURLOPT_COOKIEFILE,"~/.cache/rchipcookies");
        curl_easy_setopt(session,CURLOPT_WRITEDATA,NULL);
        curl_easy_perform(session);
    }
    g_free(url);
    curl_easy_cleanup(session);
}
#endif //#ifdef _JSON
