#include <config.h>

#ifndef _WIN32
	#include 	"dbus.h"
	#include	"utils.h"
	#include	"notifications.h"
	#include        <stdio.h>
	#include	<stdlib.h>
	#include	<gio/gio.h>	
	#include	<glib.h>
	#include	<string.h>	
	/*we set the dbus timeout to 3 seconds*/
	#define		DBUS_TIMEOUT	3000
	/* two proxies and the connection*/
	#ifdef RHYTHMBOX
	static GDBusProxy *player = 0;
	static GDBusProxy *shell = 0;
	#endif
	#ifdef BANSHEE
	static GDBusProxy *playerEngine = 0;
	static GDBusProxy *playbackController = 0;
	#endif
	static GDBusConnection *conn = NULL;
	
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
		conn = g_bus_get_sync(G_BUS_TYPE_SESSION,NULL,&error);
		if (error != NULL) {
			#if VERBOSE >= 1
			printf("Somethings wrong:dbus_g_bus_get\n");
			printf("%s\n",error->message);
			#endif
			conn = NULL;
			return FALSE;
		}
		#ifdef RHYTHMBOX
		shell = g_dbus_proxy_new_sync(conn, G_DBUS_CALL_FLAGS_NONE,NULL,"org.gnome.Rhythmbox",
                                                     "/org/gnome/Rhythmbox/Shell",
                                                      "org.gnome.Rhythmbox.Shell",NULL,&error);
		if (error != NULL) {
                        #if VERBOSE >= 1
                        printf("Somethings wrong:dbus_g_proxy_new_for_name_owner(SHELL)\n");
                        printf("%s\n",error->message);
                        #endif
                        conn =NULL;
                        return FALSE;
                }
		player = g_dbus_proxy_new_sync(conn, G_DBUS_CALL_FLAGS_NONE,NULL,"org.gnome.Rhythmbox",
                                                     "/org/gnome/Rhythmbox/Player",
                                                      "org.gnome.Rhythmbox.Player",NULL,&error);     
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
                playerEngine = g_dbus_proxy_new_sync(conn, G_DBUS_CALL_FLAGS_NONE,NULL,
                                                        "org.bansheeproject.Banshee",
                                                        "/org/bansheeproject/Banshee/PlayerEngine",
                                                        "org.bansheeproject.Banshee.PlayerEngine",NULL,&error);
                if (error != NULL) {
                        #if VERBOSE >= 1
                        printf("Somethings wrong:dbus_g_proxy_new_for_name_owner(PlayerEngine)\n");
                        printf("%s\n",error->message);
                        #endif
                        conn =NULL;
                        return FALSE;
                }
		playbackController = g_dbus_proxy_new_sync(conn, G_DBUS_CALL_FLAGS_NONE,NULL,
                                                        "org.bansheeproject.Banshee",
                                                        "/org/bansheeproject/Banshee/PlaybackController",
                                                        "org.bansheeproject.Banshee.PlaybackController",NULL,&error);
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
		g_dbus_proxy_call_sync(player,command_name,NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
		#endif
		#ifdef BANSHEE
                g_dbus_proxy_call_sync(playerEngine,command_name,NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
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
	
	gboolean send_command_to_music_player_with_argument(char* command_name,char* type, char* argument) {
	        GError *error = NULL;
		#ifdef RHYTHMBOX
		g_dbus_proxy_call_sync(player,command_name,g_variant_new (type,&argument),G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
		#endif
                #ifdef BANSHEE
		g_dbus_proxy_call_sync(playbackController,command_name,g_variant_new (type,&argument),G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
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
	
	struct playing_info_music dbus_get_playing_info_music() {
		struct playing_info_music pInfo;
		GError *error = NULL;
		if (dbus_is_connected(TRUE)) {
			#ifdef RHYTHMBOX
			gboolean playing;
			GVariant* results =g_dbus_proxy_call_sync(player,"getPlaying",NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
			if (error != NULL) {
				#if VERBOSE >= 1
				printf("Error with getPlaying: %s\n",error->message);
				#endif
			}
			playing = g_variant_get_boolean(g_variant_get_child_value(results,0));
			if (playing){
				results = NULL;
				results =g_dbus_proxy_call_sync(player,"getPlayingUri",NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
				if (error == NULL) {
					const char* uri = g_variant_get_string(g_variant_get_child_value(results,0),NULL);
					results = NULL;
					results =g_dbus_proxy_call_sync(shell,"getSongProperties",g_variant_new ("(s)",uri),G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
					char *tempval = malloc (8192);
	                                double doubleValue;
	                                GVariant* dict = g_variant_get_child_value(results,0);
	                                g_variant_lookup(dict,"artist","s",&tempval);
	                               	pInfo.Artist = malloc(strlen(tempval));
					strcpy(pInfo.Artist,tempval); 
					g_variant_lookup(dict,"album","s",&tempval);
					pInfo.Album = malloc(strlen(tempval));
	                                strcpy(pInfo.Album,tempval);
	                                g_variant_lookup(dict,"title","s",&tempval);
					pInfo.Song = malloc(strlen(tempval));
	                                strcpy(pInfo.Song,tempval);
	                                g_variant_lookup(dict,"duration","d",&doubleValue);
					free(tempval);
	                                pInfo.Duration = doubleValue;
					pInfo.isPlaying=playing;
					results =g_dbus_proxy_call_sync(player,"getElapsed",NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
					pInfo.Elapised_time = g_variant_get_uint32(g_variant_get_child_value(results,0));
				} else {
	                        	#if VERBOSE >= 1
	                                printf("ERROR:%s\n",error->message);
	                                #endif
	                        }
			} else {
				pInfo.Artist = "";
				pInfo.Album = "";
				pInfo.Song = "";
				pInfo.Duration = 0;
				pInfo.Elapised_time = 0;
			}
			#endif
			
			#ifdef BANSHEE
                        GVariant* result = g_dbus_proxy_call_sync(playerEngine,"GetCurrentState",NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
			if (strcmp(g_variant_get_string(g_variant_get_child_value(result,0),NULL),"playing") != 0){
				pInfo.isPlaying=FALSE;
				pInfo.Artist = "";
                                pInfo.Album = "";
                                pInfo.Song = "";
                                pInfo.Duration = 0;
                                pInfo.Elapised_time = 0;
			} else {
				pInfo.isPlaying=TRUE;
				result = g_dbus_proxy_call_sync(playerEngine,"GetPosition",NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
                                unsigned int c = g_variant_get_uint32(g_variant_get_child_value(result,0));
                                result = g_dbus_proxy_call_sync(playerEngine,"GetLength",NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
                                unsigned int t = g_variant_get_uint32(g_variant_get_child_value(result,0));
                                pInfo.Elapised_time = (c*pInfo.Duration)/t;
				result =g_dbus_proxy_call_sync(playerEngine,"GetCurrentTrack",NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
				char *tempval1 = malloc (8192);
				double doubleValue;
                                GVariant* dict = g_variant_get_child_value(result,0);
                                g_variant_lookup(dict,"artist","s",&tempval1);
                                pInfo.Artist = malloc(strlen(tempval1));
                                strcpy(pInfo.Artist,tempval1);
                                free(tempval1);
				char *tempval2 = malloc (8192);
				g_variant_lookup(dict,"album","s",&tempval2);
                                pInfo.Album = malloc(strlen(tempval2));
                                strcpy(pInfo.Album,tempval2);
				free(tempval2);
                                char *tempval3 = malloc (8192);
				g_variant_lookup(dict,"name","s",&tempval3);
                                pInfo.Song = malloc(strlen(tempval3));
                                strcpy(pInfo.Song,tempval3);
                                free(tempval3);
                                g_variant_lookup(dict,"length","d",&doubleValue);
                                pInfo.Duration = doubleValue;

			}
		#endif
		}
		return pInfo;	
	}
	
	void print_playing_info_music(const struct playing_info_music pInfo){
		#if VERBOSE >= 4
		printf("Artist\t\t%s\nAlbum\t\t%s\nSong\t\t%s\nElapised Time\t%i\nDuration\t%i\nIs Playing\t%i\n\n",pInfo.Artist,pInfo.Album,pInfo.Song,pInfo.Elapised_time,pInfo.Duration,pInfo.isPlaying);
		#endif
	}
#endif
