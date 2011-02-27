#include <config.h>

#ifndef _WIN32
	#include 	"dbus.h"
	#include	"notifications.h"
	#include 	<glib.h>
	#include	<glib-object.h>
	#include	<dbus/dbus.h>
	#include	<dbus/dbus-glib.h>
	#include        <string.h>
	#include        <stdio.h>
	
	/*we set the dbus timeout to 3 seconds*/
	#define		DBUS_TIMEOUT	3000
	/* two proxies and the connection*/
	#ifdef RHYTHMBOX
	static DBusGProxy *player = 0;
	static DBusGProxy *shell = 0;
	#endif
	#ifdef BANSHEE
	static DBusGProxy *playerEngine = 0;
	static DBusGProxy *playbackController = 0;
	#endif
	static DBusGConnection *conn = NULL;
	
	/*  
	 *  if connection(conn) is null, and we tryOnDisconnced is true it try and reconnect. 
	 *  if conn is not null it return true
	 *  else it returns false
	 */
	
	gboolean dbus_is_connected (gboolean tryOnDisconneced) {
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
	
	gboolean dbus_init(){
		GError *error = NULL;
		conn = dbus_g_bus_get(DBUS_BUS_SESSION,&error);
		if (error != NULL) {
			#if VERBOSE >= 1
			printf("Somethings wrong:dbus_g_bus_get\n");
			printf("%s\n",error->message);
			#endif
			conn = NULL;
			return FALSE;
		}
		#ifdef RHYTHMBOX		
	        shell = dbus_g_proxy_new_for_name_owner(conn,	
                	                              "org.gnome.Rhythmbox",
 	                                             "/org/gnome/Rhythmbox/Shell",
        	                                      "org.gnome.Rhythmbox.Shell",&error);
		if (error != NULL) {
                        #if VERBOSE >= 1
                        printf("Somethings wrong:dbus_g_proxy_new_for_name_owner(SHELL)\n");
                        printf("%s\n",error->message);
                        #endif
                        conn =NULL;
                        return FALSE;
                }
       		player = dbus_g_proxy_new_for_name_owner(conn,
                	                               "org.gnome.Rhythmbox",
                        	                       "/org/gnome/Rhythmbox/Player",
                                	               "org.gnome.Rhythmbox.Player",&error);
		if (error != NULL) {
			#if VERBOSE >= 1
                	printf("Somethings wrong:dbus_g_proxy_new_for_name_owner(PLAYER)\n");
	                printf("%s\n",error->message);
			#endif
			conn = NULL;
	                return FALSE;
	        }
		#endif
		#ifdef BANSHEE
                playerEngine = dbus_g_proxy_new_for_name_owner(conn,
                                                        "org.bansheeproject.Banshee",
                                                        "/org/bansheeproject/Banshee/PlayerEngine/",
                                                        "org.bansheeproject.Banshee.PlayerEngine",&error);
                if (error != NULL) {
                        #if VERBOSE >= 1
                        printf("Somethings wrong:dbus_g_proxy_new_for_name_owner(PlayerEngine)\n");
                        printf("%s\n",error->message);
                        #endif
                        conn =NULL;
                        return FALSE;
                }
		playbackController = dbus_g_proxy_new_for_name_owner(conn,
                                                        "org.bansheeproject.Banshee",
                                                        "/org/bansheeproject/Banshee/PlaybackController/",
                                                        "org.bansheeproject.Banshee.PlaybackController",&error);
                if (error != NULL) {
                        #if VERBOSE >= 1
                        printf("Somethings wrong:dbus_g_proxy_new_for_name_owner(PlaybackController)\n");
                        printf("%s\n",error->message);
                        #endif
                        conn =NULL;
                        return FALSE;
                }

                #endif
		return TRUE;
	}
	gboolean send_command_to_music_player(char* command_name) {
		GError *error = NULL;
		#ifdef RHYTHMBOX
		dbus_g_proxy_call_with_timeout(player,command_name, DBUS_TIMEOUT, &error, G_TYPE_INVALID, G_TYPE_INVALID);
		#endif
		#ifdef BANSHEE
                dbus_g_proxy_call_with_timeout(playerEngine,command_name, DBUS_TIMEOUT, &error, G_TYPE_INVALID, G_TYPE_INVALID);
                #endif

		if (error != NULL) {
	         	#if VERBOSE >= 1
	                printf("Error with getPlaying: %s\n",error->message);
	                #endif
			return FALSE;
		 } else {
			return TRUE;
		}
	}
	
	gboolean send_command_to_music_player_with_argument(char* command_name,GType type, char* argument) {
		printf("We did something: %s\n",command_name);
	        GError *error = NULL;
		#ifdef RHYTHMBOX
		dbus_g_proxy_call_with_timeout(player,command_name, DBUS_TIMEOUT, &error,type,argument, G_TYPE_INVALID, G_TYPE_INVALID);
		#endif
                #ifdef BANSHEE
		dbus_g_proxy_call_with_timeout(playbackController,command_name, DBUS_TIMEOUT, &error,type,argument, G_TYPE_INVALID, G_TYPE_INVALID);
		#endif
		if (error != NULL) {
	                #if VERBOSE >= 1
	                printf("Error with getPlaying: %s\n",error->message);
        	        #endif
        	        return FALSE;
        	 } else {
	                return TRUE;
       	 	 }
	}

	void print_address_key_and_value (char *key, char *value, gpointer user_data)
{
        g_print ("    %s: %s\n", key, value);
}	

	struct playing_info_music dbus_get_playing_info_music() {
		struct playing_info_music pInfo = {"Artist","Album","Song",0,3600,0};
		GError *error = NULL;
		if (dbus_is_connected(TRUE)) {
			#ifdef RHYTHMBOX	
			gboolean playing;
			dbus_g_proxy_call_with_timeout(player, "getPlaying", DBUS_TIMEOUT, &error,
	        	                        G_TYPE_INVALID,
	        	                        G_TYPE_BOOLEAN,&playing ,
	        	                        G_TYPE_INVALID);
			pInfo.isPlaying=playing;
			if (error != NULL) {
				#if VERBOSE >= 1
				printf("Error with getPlaying: %s\n",error->message);
				#endif
			}	
			GHashTable *table;
			char* uri = "";
			dbus_g_proxy_call_with_timeout(player, "getPlayingUri", DBUS_TIMEOUT, &error,G_TYPE_INVALID, G_TYPE_STRING, &uri, G_TYPE_INVALID);
			if (error == NULL) {
				dbus_g_proxy_call_with_timeout(shell, "getSongProperties", DBUS_TIMEOUT, &error,G_TYPE_STRING, uri, G_TYPE_INVALID, dbus_g_type_get_map("GHashTable", G_TYPE_STRING, G_TYPE_VALUE), &table, G_TYPE_INVALID);	
				if (error == NULL) {
					pInfo.Song= get_hash_str(table, "title");
					pInfo.Artist=get_hash_str(table, "artist");
		        		pInfo.Album=get_hash_str(table, "album");
	        			pInfo.Duration = get_hash_uint(table, "duration");
					dbus_g_proxy_call_with_timeout(player, "getElapsed", DBUS_TIMEOUT, NULL,G_TYPE_INVALID,G_TYPE_UINT, &pInfo.Elapised_time,  G_TYPE_INVALID);
				} else {
	        	                #if VERBOSE >= 1
					printf("ERROR:%s\n",error->message);
					#endif
				}
			}
			#endif
			#ifdef BANSHEE
			/*gboolean playing;
                        dbus_g_proxy_call_with_timeout(playerEngine, "GetCurrentTrack", DBUS_TIMEOUT, &error,
                                                G_TYPE_INVALID,
                                                G_TYPE_BOOLEAN,,
                                                G_TYPE_INVALID);
                        pInfo.isPlaying=playing;
			*/
			char* uri = "";
			GHashTable *map = NULL;
//			DBusGProxy *property = dbus_g_proxy_new_for_name (conn,"org.bansheeproject.Banshee", "/org/bansheeproject/Banshee/PlayerEngine/","org.freedesktop.DBus.Properties");
//			dbus_g_proxy_call_with_timeout(property, "Get",DBUS_TIMEOUT, &error,G_TYPE_STRING, "org.bansheeproject.Banshee.PlayerEngine", G_TYPE_INVALID,G_TYPE_STRING, "CurrentTrack",dbus_g_type_get_map("GHashTable", G_TYPE_STRING, G_TYPE_VALUE), &map, G_TYPE_INVALID);

//			g_hash_table_foreach(map,(GHFunc)print_address_key_and_value,NULL);
			if (error == NULL){

			} else {
				#if VERBOSE >= 1
				printf("ERROR:%s\n",error->message);
				#endif
			}
			
			#endif
		}		
		return pInfo;	
	}
	

	char* get_hash_str(GHashTable *table, const char *key){
		GValue* value = (GValue*) g_hash_table_lookup(table, key);
	        if (value != NULL && G_VALUE_HOLDS_STRING(value)) {
			const char* src = g_value_get_string(value);	
			#if VERBOSE >= 4
			printf("Got info for key '%s' is '%s'\n", key, src);
			#endif
                	return src;
        	}	
     		return "FALSE";
	}

	unsigned int get_hash_uint(GHashTable *table, const char *key){
		GValue* value = (GValue*) g_hash_table_lookup(table, key);
		if (value != NULL && G_VALUE_HOLDS_UINT(value)) {
			return g_value_get_uint(value);
		}
		return 0;
	}
	
	void print_playing_info_music(const struct playing_info_music pInfo){
		#if VERBOSE >= 4
		printf("Artist\t\t%s\nAlbum\t\t%s\nSong\t\t%s\nElapised Time\t%i\nDuration\t%i\nIs Playing\t%i\n\n",pInfo.Artist,pInfo.Album,pInfo.Song,pInfo.Elapised_time,pInfo.Duration,pInfo.isPlaying);
		#endif
	}
#endif
