/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
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

#ifdef _JSON

#include	<curl/curl.h>
#include	<json/json.h>
#include 	<glib.h>
#include	<glib/gprintf.h>
#include	<string.h>
#include	<stdlib.h>
#include 	"rest.h"
#include	"settings.h"
#include	"utils.h"
#include	"cmdhandler.h"
#include 	<sys/utsname.h>

char* URL = "http://192.168.1.3/json";
CURL *session;
void get_cmd_from_server(char *hostname) {
	if (session) {
		char* url = g_strconcat(URL,"/getcommand/?host=",curl_easy_escape(session,hostname,strlen(hostname)),NULL);
	    curl_easy_setopt(session, CURLOPT_URL, url);
		curl_easy_setopt(session, CURLOPT_COOKIEFILE,"~/.cache/rchipcookies");
		curl_easy_setopt(session, CURLOPT_WRITEFUNCTION, get_commands_callback);
		curl_easy_perform(session);
		g_free(url);
	}
}

void send_cmd_to_server(char* hostname,char* cmd,char* cmdTxt){
	struct utsname uts;
	uname( &uts );
	if (session) {
		char* url = g_strconcat(URL,"/sendcommand/?command=",curl_easy_escape(session,cmd,strlen(cmd)),"&command_text=",curl_easy_escape(session,cmdTxt,strlen(cmdTxt)),"&source_hostname=",curl_easy_escape(session,uts.nodename,strlen(uts.nodename)),"&destination_hostname=",curl_easy_escape(session,hostname,strlen(hostname)),NULL);
		curl_easy_setopt(session, CURLOPT_URL, url);
		curl_easy_setopt(session, CURLOPT_COOKIEFILE,"~/.cache/rchipcookies");
		curl_easy_perform(session);
	}
}
size_t get_commands_callback(void *ptr,size_t size, size_t count, void* stream){
	printf("ptr:%s:\n",ptr);
	if (g_strcmp0(ptr,"\"Not Authorized\"") == 0){
		printf("Error with Authentication\n");
		return 0;
	}
	json_object *jarray = json_tokener_parse((char*)ptr);
	for (int i = 0; i< json_object_array_length(jarray); i++){
		char *cmd;
		char *cmd_txt;
		json_object_object_foreach(json_object_array_get_idx(jarray, i), key, val){
			if (g_strcmp0(key,"command") == 0){
				cmd = json_object_get_string(val);
			}
			if (g_strcmp0(key,"command_text") == 0){
				cmd_txt = json_object_get_string(val);
			}
		}
		printf("Command=%s\n",cmd);
		process_cmd(cmd,cmd_txt);	
	}
	return 0;
}

char* get_registered_devices_message(){
	return NULL;
}

void get_active_devices( void ){
	return;
	if (session) {
		char* url = g_strconcat(URL,"/getremotedevice/",NULL);
		printf("url:%s\n",url);
	    curl_easy_setopt(session, CURLOPT_URL, url);
		curl_easy_setopt(session, CURLOPT_COOKIEFILE,"~/.cache/rchipcookies");
		curl_easy_setopt(session, CURLOPT_WRITEFUNCTION, get_active_decives_callback);
		curl_easy_perform(session);
		g_free(url);
	}
}

size_t get_active_decives_callback(void *ptr,size_t size, size_t count, void* stream){
	//json_object *jarray = json_tokener_parse((char*)ptr);
	json_object *jarray = json_tokener_parse("[{\"devices_name\": \"7327943473\"}]");
	for (int i = 0; i< json_object_array_length(jarray); i++){
		json_object_object_foreach(json_object_array_get_idx(jarray, i), key, val){
			Hosts->add(json_object_get_string(val));
		}
	}
	return 0;
}
void update_daemon_server(){
}

void set_song_info_rest(struct playing_info_music pInfo, char* hostname) {
	if (pInfo.isPlaying != 0){
		if (session) {
			char* url = g_strdup_printf("%s/setsonginfo/?artist=%s&album=%s&song=%s&elapsed_time=%d&total_time=%d&is_playing=%d&dest_hostname=%s",URL,curl_easy_escape(session,pInfo.Artist,strlen(pInfo.Artist)),curl_easy_escape(session,pInfo.Album,strlen(pInfo.Album)),curl_easy_escape(session,pInfo.Song,strlen(pInfo.Song)),pInfo.Elapised_time,pInfo.Duration,pInfo.isPlaying,curl_easy_escape(session,hostname,strlen(hostname)));
			printf("url:%s\n",url);
			curl_easy_setopt(session, CURLOPT_URL, url);
			curl_easy_setopt(session, CURLOPT_COOKIEFILE,"~/.cache/rchipcookies");
			curl_easy_setopt(session, CURLOPT_WRITEFUNCTION, get_active_decives_callback);
			curl_easy_perform(session);
			g_free(url);
		}
	}
}

void authenticate( void ){
	char* url = g_strconcat(URL,"/authenticate/?username=kevin&password=Inverse81",NULL);
	char* response;
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
