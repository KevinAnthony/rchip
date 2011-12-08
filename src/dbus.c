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

#include 	"dbus.h"
#include	"utils.h"
#include	"notifications.h"
#include	"xml.h"
#include	<string.h>
#include	<gio/gio.h>	
#include	<glib.h>
#include	<glib/gprintf.h>
#include	<assert.h>
/*we set the dbus timeout to 3 seconds*/
#define		DBUS_TIMEOUT	3000
/* two proxies and the connection*/
static GDBusProxy *musicProxy = NULL;
static GDBusProxy *videoProxy = NULL;
static GDBusConnection *conn = NULL;
guint watcher_id_music = 0;
guint watcher_id_video = 0;

static char* currentMusicObject = NULL;
static char* currentVideoObject = NULL;

static gboolean musicConnected = FALSE;
static gboolean videoConnected = FALSE;

/*
 * if connection(conn) is null, and we tryOnDisconnced is true it try and reconnect. 
 * if conn is not null it return true
 * else it returns false
 */
gboolean dbus_is_connected (gboolean tryOnDisconneced) {
	if (!(musicConnected && videoConnected)){
		dbus_init();
	}
	if (conn != NULL) {
		return TRUE;
	} else {
		if (tryOnDisconneced) {
			return dbus_init();
		} else {
			return FALSE;
		}
	}
}

/* init the dbus */
gboolean dbus_init(){
	GError *error = NULL;
	/* Get the session bus */
	conn = g_bus_get_sync(G_BUS_TYPE_SESSION,NULL,&error);
		if (error != NULL) {
		#if VERBOSE >= 1
		g_error("Somethings wrong:dbus_g_bus_get\n");
		g_error("%s\n",error->message);
		#endif
		conn = NULL;
		return FALSE;
	}
	char* busName = NULL;
	busName = xml_get_bus_name("MUSIC");
	char* objectPath = NULL;
	objectPath = xml_get_bus_path("MUSIC");
	currentMusicObject = objectPath;
	if (watcher_id_music == 0){
		watcher_id_music = g_bus_watch_name(G_BUS_TYPE_SESSION,busName,G_BUS_NAME_WATCHER_FLAGS_NONE,on_name_appeared, on_name_vanished, NULL, NULL);
	}
	if (musicConnected){
		if ((busName != NULL) && (objectPath != NULL)){
			musicProxy= g_dbus_proxy_new_sync(conn, G_DBUS_CALL_FLAGS_NONE,NULL,busName,objectPath,busName,NULL,&error);
			if (error != NULL) {
				#if VERBOSE >= 1
				g_error("Somethings wrong:dbus_g_proxy_new_for_name_owner(SHELL)\n");
				g_error("%s\n",error->message);
				#endif
				conn =NULL;
				return FALSE;
			}
		}
	}
	if (busName != NULL){
		g_free(busName);
		busName = NULL;
	}
	if (objectPath != NULL){
		g_free(objectPath);
		objectPath = NULL;
	}
	busName = xml_get_bus_name("VIDEO");
	objectPath = xml_get_bus_path("VIDEO");
	if ((watcher_id_video == 0) && (busName != NULL)){
                watcher_id_video = g_bus_watch_name(G_BUS_TYPE_SESSION,busName,G_BUS_NAME_WATCHER_FLAGS_NONE,on_name_appeared, on_name_vanished, NULL, NULL);
	}
        if (videoConnected){
		if ((busName != NULL) && (objectPath != NULL)){
			videoProxy= g_dbus_proxy_new_sync(conn, G_DBUS_CALL_FLAGS_NONE,NULL,busName,objectPath,busName,NULL,&error);
			if (error != NULL) {
				#if VERBOSE >= 1
				g_error("Somethings wrong:dbus_g_proxy_new_for_name_owner(SHELL)\n");
				g_error("%s\n",error->message);
				#endif
				conn =NULL;
				return FALSE;
			}
		}
	}
	if (busName != NULL){
                g_free(busName);
                busName = NULL;
        }
        if (objectPath != NULL){
                g_free(objectPath);
                objectPath = NULL;
        }

	return TRUE;
}

gboolean new_proxy(char* type, char* busName,char* objectPath){
	assert(busName != NULL);
	assert(objectPath != NULL);
	GError *error = NULL;
	if (g_strcmp0(type,"MUSIC") == 0){
		musicProxy= g_dbus_proxy_new_sync(conn, G_DBUS_CALL_FLAGS_NONE,NULL,busName,objectPath,busName,NULL,&error);
		if (error != NULL) {
			#if VERBOSE >= 1
			g_error("Somethings wrong:dbus_g_proxy_new_for_name_owner(SHELL)\n");
			g_error("%s\n",error->message);
			#endif
			conn =NULL;
			return FALSE;
		}
		currentMusicObject = objectPath;
	} else if (g_strcmp0(type,"video") == 0){
		videoProxy= g_dbus_proxy_new_sync(conn, G_DBUS_CALL_FLAGS_NONE,NULL,busName,objectPath,busName,NULL,&error);
		if (error != NULL) {
			#if VERBOSE >= 1
			g_error("Somethings wrong:dbus_g_proxy_new_for_name_owner(SHELL)\n");
			g_error("%s\n",error->message);
			#endif
			conn =NULL;
			return FALSE;
		}
		currentVideoObject = objectPath;
	}
	return TRUE;
}

gchar* get_object (gchar* incoming_command){
	gchar* q = g_strdup(incoming_command);
	gchar* retval = NULL;
	gchar* temp = NULL;
	gchar* p = NULL;
	int numOfDots = 0;
	int len = 0;
	int wordLen = 0;
	p = q;
	while (*p != '\0'){
		if (*p == '.'){
			*p = '/';
			numOfDots++;
			wordLen = -1;
		}
		len++;
		wordLen++;
		p++;
	}
	len = len - wordLen - 1;
	temp = g_strndup(q,len);
	retval = g_strdup_printf("/%s",temp);
	g_free(temp);
	g_free(q);
	return retval;
}



gboolean send_command_to_music_player(char* command_name) {
	if (musicConnected){
		GError *error = NULL;
		char* incomingObject = get_object(command_name);
		if (g_strcmp0(incomingObject,currentMusicObject) != 0 ){
			new_proxy("MUSIC",xml_get_bus_name("MUSIC"),incomingObject);
			g_free(incomingObject);
		}
		g_dbus_proxy_call_sync(musicProxy,command_name,NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
		if (error != NULL) {
		 	#if VERBOSE >= 1
			g_error("Error with getPlaying: %s\n",error->message);
			#endif
			return FALSE;
		 } else {
			return TRUE;
		}
	} else {
		return FALSE;
	}
}
gboolean send_command_to_music_player_with_argument(char* command_name,char* type, char* argument) {
	if (musicConnected){
		GError *error = NULL;
		char* incomingObject = get_object(command_name);
		if (g_strcmp0(incomingObject,currentMusicObject) != 0 ){
	                new_proxy("MUSIC",xml_get_bus_name("MUSIC"),incomingObject);
	                g_free(incomingObject);
	        }
		printf("commandName: %s\n",command_name);
		g_dbus_proxy_call_sync(musicProxy,command_name,g_variant_new (type,&argument),G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
		if (error != NULL) {
			#if VERBOSE >= 1
			g_error("Error with getPlaying: %s\n",error->message);
			#endif
			return FALSE;
		 } else {
			return TRUE;
		}
	} else {
                return FALSE;
	}
}

gboolean send_command_to_video_player(char* command_name) {
	if (musicConnected){
		GError *error = NULL;
		g_dbus_proxy_call_sync(videoProxy,command_name,NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
		if (error != NULL) {
			#if VERBOSE >= 1
			g_error("Error with getPlaying: %s\n",error->message);
			#endif
			return FALSE;
		} else {
			return TRUE;
		}
	} else {
                return FALSE;
        }
}
gboolean send_command_to_video_player_with_argument(char* command_name,char* type, char* argument) {
	if (musicConnected){
		GError *error = NULL;
		g_dbus_proxy_call_sync(videoProxy,command_name,g_variant_new (type,&argument),G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
		if (error != NULL) {
			#if VERBOSE >= 1
			g_error("Error with getPlaying: %s\n",error->message);
			#endif
			return FALSE;
		} else {
			return TRUE;
		}
	} else {
                return FALSE;
        }
}
	/* get playing info from music player */
struct playing_info_music dbus_get_playing_info_music() {
	struct playing_info_music pInfo = {"Artist","Album","Song",0,0,0};
	if (musicConnected){
		GError *error = NULL;
		/* Make sure we are connected to dbus */
		if (dbus_is_connected(TRUE)) {
			/* 
			 * dbus and rhythmbox are raticlly diffrent here
			 * I don't know if we can combind this into one command, but i should look into it
			 */
			#ifdef RHYTHMBOX	
			gboolean playing;
			char* stateObject = "/org/gnome/Rhythmbox/Player";
	                if (g_strcmp0(stateObject,currentMusicObject) != 0 ){
	                        new_proxy("MUSIC",xml_get_bus_name("MUSIC"),stateObject);
	                }
			/* Check to see if the player is playing, if it's not, no point going any further*/
			GVariant* results =g_dbus_proxy_call_sync(musicProxy,"org.gnome.Rhythmbox.Player.getPlaying",NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
			if (error != NULL) {
				#if VERBOSE >= 1
				g_error("Error with getPlaying: %s\n",error->message);
				#endif
			}
			playing = g_variant_get_boolean(g_variant_get_child_value(results,0));
			if (playing){
				/* Get the current playing URI so we can get it's properties*/
				results =g_dbus_proxy_call_sync(musicProxy,"org.gnome.Rhythmbox.Player.getPlayingUri",NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
				if (error == NULL) {
					/* TODO: we need to check that results <> NULL */
					const char* uri = g_variant_get_string(g_variant_get_child_value(results,0),NULL);
					new_proxy("MUSIC",xml_get_bus_name("MUSIC"),"/org/gnome/Rhythmbox/Shell/");
					/* using the URI above, we need to the the songPorperties */
					results =g_dbus_proxy_call_sync(musicProxy,"org.gnome.Rhythmbox..Shell.getSongProperties",g_variant_new ("(s)",uri),G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
					new_proxy("MUSIC",xml_get_bus_name("MUSIC"),stateObject);
					/* TODO: we need to check the error and that result is not NULL */
					double doubleValue;
					GVariant* dict = g_variant_get_child_value(results,0);
					/* We don't know how long the information is, so we allocate 8K of chariters */
					char* tempVar=g_malloc(8192);
					/* 
					 * Each segment below is similar
					 * we pull the infomation out of the dict and into tempVar
					 * we then allocate the pInfo field using the length +1(for the \0 char
					 */
					g_variant_lookup(dict,"artist","s",&tempVar);
					pInfo.Artist = g_strdup(tempVar);
					g_free(tempVar);
	
					tempVar=g_malloc(8192);
					g_variant_lookup(dict,"album","s",&tempVar);
					pInfo.Album = g_strdup(tempVar);
					g_free(tempVar);
	
					tempVar=g_malloc(8192);
					g_variant_lookup(dict,"title","s",&tempVar);
					pInfo.Song = g_strdup(tempVar);
					g_free(tempVar);
	
					g_variant_lookup(dict,"duration","d",&doubleValue);
					pInfo.Duration = doubleValue;
					pInfo.isPlaying=playing;
					/* we get the elaped time playing*/
					results =g_dbus_proxy_call_sync(musicProxy,"org.gnome.Rhythmbox.Player.getElapsed",NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
					pInfo.Elapised_time = g_variant_get_uint32(g_variant_get_child_value(results,0));
				} else {
					#if VERBOSE >= 1
					g_error("ERROR:%s\n",error->message);
					#endif
				}
			} else {
				/* if playing is false, or we are not playing, set values to empty */
				pInfo.Artist = "";
				pInfo.Album = "";
				pInfo.Song = "";
				pInfo.isPlaying=playing;
				pInfo.Elapised_time = 0;
				pInfo.Duration = 0;
			}
			#endif
			#ifdef BANSHEE
			/* GetCurrentState returns playing or not playing */
			char* stateObject = "/org/bansheeproject/Banshee/PlayerEngine";
		        if (g_strcmp0(stateObject,currentMusicObject) != 0 ){
	        	        new_proxy("MUSIC",xml_get_bus_name("MUSIC"),stateObject);
	        	}
			GVariant* result = g_dbus_proxy_call_sync(musicProxy,"org.bansheeproject.Banshee.PlayerEngine.GetCurrentState",NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
			if (error == NULL){
				/* if we are not playing set playing = true, else get the res of the info*/
				if (strcmp(g_variant_get_string(g_variant_get_child_value(result,0),NULL),"playing") != 0){
					pInfo.isPlaying=FALSE;
					pInfo.Artist = "";
					pInfo.Album = "";
					pInfo.Song = "";
					pInfo.Elapised_time = 0;
					pInfo.Duration = 0;
				} else {
					pInfo.isPlaying=TRUE;
					/* We get the current playing information*/
					result =g_dbus_proxy_call_sync(musicProxy,"org.bansheeproject.Banshee.PlayerEngine.GetCurrentTrack",NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
					GVariant* dict = g_variant_get_child_value(result,0);
					double doubleValue;
					/* this works like like loopup above*/
					char* tempVar = g_malloc(8192);
					g_variant_lookup(dict,"artist","s",&tempVar);
					pInfo.Artist = g_strdup(tempVar);
					g_free(tempVar);
	
					tempVar=g_malloc(8192);
					g_variant_lookup(dict,"album","s",&tempVar);
					pInfo.Album = g_strdup(tempVar);
					g_free(tempVar);
		
					tempVar=g_malloc(8192);
					g_variant_lookup(dict,"name","s",&tempVar);
					pInfo.Song = g_strdup(tempVar);
					g_free(tempVar);
	
					g_variant_lookup(dict,"length","d",&doubleValue);
					pInfo.Duration = doubleValue;
					/* 
					 * this is tricky, c and t are current posistion and total length in bytes 
					 * we use basic algebra to get the position in secounds using cross-multiply and divide 
					 */
					result = g_dbus_proxy_call_sync(musicProxy,"org.bansheeproject.Banshee.PlayerEngine.GetPosition",NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
					unsigned int c = g_variant_get_uint32(g_variant_get_child_value(result,0));
					result = g_dbus_proxy_call_sync(musicProxy,"org.bansheeproject.Banshee.PlayerEngine.GetLength",NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
					unsigned int t = g_variant_get_uint32(g_variant_get_child_value(result,0));
					pInfo.Elapised_time = (c*pInfo.Duration)/t;
				}
			} else {
				#if VERBOSE >= 1
				g_warning("problem with banshee get Current State::%s\n",error->message);
				#endif
			}
			#endif
		}
	}
	return pInfo;	
}

/* This outputs the information in human readable */
void print_playing_info_music(const struct playing_info_music pInfo){
	if ( pInfo.Artist == NULL ){
		#if VERBOSE >= 4
		g_printf("Artist\t\t%s\nAlbum\t\t%s\nSong\t\t%s\nElapised Time\t%i\nDuration\t%i\nIs Playing\t%i\n\n",pInfo.Artist,pInfo.Album,pInfo.Song,pInfo.Elapised_time,pInfo.Duration,pInfo.isPlaying);
		#endif
	}
}

void on_name_appeared (GDBusConnection *connection, const gchar *name, const gchar *name_owner, gpointer user_data)
{
	#if VERBOSE >= 3
  		g_print ("Name %s on %s is owned by %s\n", name,"the session bus", name_owner);
	#endif
	if (g_strcmp0(name,xml_get_bus_path("MUSIC"))){
		musicConnected = TRUE;	
	} else if (g_strcmp0(name,xml_get_bus_path("VIDEO"))) {
		videoConnected = TRUE;
	}
	dbus_init();
}

void on_name_vanished (GDBusConnection *connection, const gchar *name, gpointer user_data)
{
	#if VERBOSE >=3
		g_print ("Name %s does not exist on %s\n", name,"the session bus");
	#endif
        if (g_strcmp0(name,xml_get_bus_path("MUSIC"))){
                musicConnected = FALSE;
        } else if (g_strcmp0(name,xml_get_bus_path("VIDEO"))) {
                videoConnected = FALSE;
        }
	dbus_init();
}
