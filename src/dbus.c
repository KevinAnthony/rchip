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
	static DBusGProxy *player = 0;
	static DBusGProxy *shell = 0;
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
			#ifndef _SILENT	
				printf("Somethings wrong:dbus_g_bus_get\n");
				printf("%s\n",error->message);
			#endif
			conn = NULL;
			return FALSE;
		}
		
	        shell = dbus_g_proxy_new_for_name_owner(conn,	
                	                              "org.gnome.Rhythmbox",
 	                                             "/org/gnome/Rhythmbox/Shell",
        	                                      "org.gnome.Rhythmbox.Shell",&error);
		if (error != NULL) {
			#ifndef _SILENT
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
			#ifndef _SILENT
	                	printf("Somethings wrong:dbus_g_proxy_new_for_name_owner(PLAYER)\n");
	                	printf("%s\n",error->message);
			#endif
			conn = NULL;
	                return FALSE;
	        }
		return TRUE;
	}
	gboolean send_command_to_rhythmbox(char* command_name) {
		GError *error = NULL;
		dbus_g_proxy_call_with_timeout(player,command_name, DBUS_TIMEOUT, &error, G_TYPE_INVALID, G_TYPE_INVALID);
		 if (error != NULL) {
	         	#ifndef _SILENT
	                	printf("Error with getPlaying: %s\n",error->message);
	                #endif
			return FALSE;
		 } else {
			return TRUE;
		}
	}
	
	gboolean send_command_to_rhythmbox_with_argument(char* command_name,GType type, char* argument) {
	        GError *error = NULL;
	        dbus_g_proxy_call_with_timeout(player,command_name, DBUS_TIMEOUT, &error,type,argument, G_TYPE_INVALID, G_TYPE_INVALID);
	         if (error != NULL) {
	                #ifndef _SILENT
	                        printf("Error with getPlaying: %s\n",error->message);
        	        #endif
        	        return FALSE;
        	 } else {
	                return TRUE;
       	 	 }
	}

	struct playing_info_rb dbus_get_playing_info_rb() {
		struct playing_info_rb pInfo = {"Artist","Album","Song",0,3600,0};
		GError *error = NULL;
		if (dbus_is_connected(TRUE)) {
				
			gboolean playing;
			dbus_g_proxy_call_with_timeout(player, "getPlaying", DBUS_TIMEOUT, &error,
	        	                        G_TYPE_INVALID,
	        	                        G_TYPE_BOOLEAN,&playing ,
	        	                        G_TYPE_INVALID);
			pInfo.isPlaying=playing;
			if (error != NULL) {
				#ifndef _SILENT
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
	        	                #ifndef _SILENT
						printf("ERROR:%s\n",error->message);
					#endif
				}
			} else {
				#ifndef _SILENT
					printf("ERROR:%s\n",error->message);
				#endif
			}
		}
		return pInfo;	
	}

	char* get_hash_str(GHashTable *table, const char *key)
	{
		GValue* value = (GValue*) g_hash_table_lookup(table, key);
	        if (value != NULL && G_VALUE_HOLDS_STRING(value)) {
			const char* src = g_value_get_string(value);	
			#ifdef _DEBUG
				printf("Got info for key '%s' is '%s'\n", key, src);
			#endif
                	return src;
        	}	
     		return "FALSE";
	}

	unsigned int get_hash_uint(GHashTable *table, const char *key)
	{
		GValue* value = (GValue*) g_hash_table_lookup(table, key);
		if (value != NULL && G_VALUE_HOLDS_UINT(value)) {
			return g_value_get_uint(value);
		}
		return 0;
	}
	
	void print_playing_info_rb(const struct playing_info_rb pInfo){
		printf("Artist\t\t%s\nAlbum\t\t%s\nSong\t\t%s\nElapised Time\t%i\nDuration\t%i\nIs Playing\t%i\n\n",pInfo.Artist,pInfo.Album,pInfo.Song,pInfo.Elapised_time,pInfo.Duration,pInfo.isPlaying);
	}
#endif
