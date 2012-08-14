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

#include    "dbus.h"
#include    "utils.h"
#include    "xml.h"
#include    <string.h>
#include    <gio/gio.h>
#include    <glib.h>
#include    <glib/gprintf.h>
#include    <assert.h>
#include    <stdlib.h>
/*we set the dbus timeout to 3 seconds*/
#define        DBUS_TIMEOUT    3000
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
        print("Somethings wrong:dbus_g_bus_get",error->message,ERROR);
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
                print("Somethings wrong:dbus_g_proxy_new_for_name_owner(SHELL)\n",error->message,ERROR);
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
                print("Somethings wrong:dbus_g_proxy_new_for_name_owner(SHELL)",error->message,ERROR);
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
            print("Somethings wrong:dbus_g_proxy_new_for_name_owner(SHELL)",error->message,ERROR);
            conn =NULL;
            return FALSE;
        }
        currentMusicObject = objectPath;
    } else if (g_strcmp0(type,"video") == 0){
        videoProxy= g_dbus_proxy_new_sync(conn, G_DBUS_CALL_FLAGS_NONE,NULL,busName,objectPath,busName,NULL,&error);
        if (error != NULL) {
            print("Somethings wrong:dbus_g_proxy_new_for_name_owner(SHELL)",error->message,ERROR);
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
            print("Error in sending command to music player",command_name,ERROR);
            print("Error Message",error->message,ERROR);
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
        g_dbus_proxy_call_sync(musicProxy,command_name,g_variant_new (type,&argument),G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
        if (error != NULL) {
            print("Error in sending command to music player",command_name,ERROR);
            print("Error Message",error->message,ERROR);
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
            print("Error in sending command to video player",command_name,ERROR);
            print("Error Message",error->message,ERROR);
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
            print("Error in sending command to video player",command_name,ERROR);
            print("Error Message",error->message,ERROR);
            return FALSE;
        } else {
            return TRUE;
        }
    } else {
        return FALSE;
    }
}
/* get playing info from music player */
playing_info_music dbus_get_playing_info_music() {
    playing_info_music pInfo = {"Artist","Album","Song",0,0,0};
    if (musicConnected){
        GError *error = NULL;
        /* Make sure we are connected to dbus */
        if (dbus_is_connected(TRUE)) {
            //new_proxy("MUSIC",xml_get_bus_name("MUSIC"),"/org/mpris/MediaPlayer2");
            //GVariant* result = g_dbus_proxy_call_sync(musicProxy,"org.mpris.MediaPlayer2.Player.Metadata",NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
            new_proxy("MUSIC",xml_get_bus_name("MUSIC"),"/org/mpris/MediaPlayer2");
            GVariant* result = g_dbus_proxy_call_sync(musicProxy,"org.freedesktop.DBus.Properties.Get",g_variant_new("(ss)","org.mpris.MediaPlayer2.Player","Metadata"),G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
            if (error != NULL) {
                print("Error Message",error->message,ERROR);
                return pInfo;
            } else {
                GVariant* dict = g_variant_get_child_value(g_variant_get_child_value(result,0),0);
                char* tempVar;
                
                GVariantIter *iter;
                GVariant *artist_value;
                artist_value = g_variant_lookup_value(dict,"xesam:artist","as");
                g_variant_get (artist_value, "as", &iter);
                g_variant_iter_loop (iter, "s", &tempVar);
                pInfo.Artist = g_strdup(tempVar);
                g_variant_iter_free (iter);
                g_variant_unref (artist_value);

                g_variant_lookup(dict,"xesam:album","s",&tempVar);
                pInfo.Album = g_strdup(tempVar);

                g_variant_lookup(dict,"xesam:title","s",&tempVar);
                pInfo.Song = g_strdup(tempVar);

                gint64 temp;
                GVariant *duration_value;
                duration_value = g_variant_lookup_value(dict,"mpris:length","x");
                temp = g_variant_get_int64(duration_value);
                temp = temp/1000000;
                pInfo.Duration = temp;
                g_variant_unref (duration_value);
                g_variant_unref (dict);
            }
            g_variant_unref (result);
            result = g_dbus_proxy_call_sync(musicProxy,"org.freedesktop.DBus.Properties.Get",g_variant_new("(ss)","org.mpris.MediaPlayer2.Player","PlaybackStatus"),G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
            if  (error != NULL) {
                print("Error Message",error->message,ERROR);
                return pInfo;
            } else {
                GVariant* varient = g_variant_get_child_value(result,0);
                GVariant* value = g_variant_get_child_value(varient,0);
                if (g_strcmp0(g_variant_get_string(value,0),"Playing") == 0)
                    pInfo.isPlaying = 1;
                else
                    pInfo.isPlaying = 0;
                g_variant_unref (varient);
                g_variant_unref (value);
                g_variant_unref (result);
            }
        } else {
            return pInfo;
        }

    }
    print_playing_info_music(pInfo);
    return pInfo;
}

/* This outputs the information in human readable */
void print_playing_info_music(const playing_info_music pInfo){
    char* tempitoa = NULL;
    print("Artist",pInfo.Artist,DEBUG);
    print("Album",pInfo.Album,DEBUG);
    print("Song Title",pInfo.Song,DEBUG);
    tempitoa = g_strdup_printf("%d",pInfo.Duration);
    print("Duration",tempitoa,DEBUG);
    g_free(tempitoa);
    tempitoa = g_strdup_printf("%d",pInfo.isPlaying);
    print("Is Playing",tempitoa,DEBUG);
    g_free(tempitoa);
}

void on_name_appeared (GDBusConnection *connection, const gchar *name, const gchar *name_owner, gpointer user_data)
{
    char* msg = g_strdup_printf ("Name %s on %s is owned by %s", name,"the session bus", name_owner);
    print(msg,NULL,INFO);
    g_free(msg);
    if (g_strcmp0(name,xml_get_bus_path("MUSIC"))){
        musicConnected = TRUE;
    } else if (g_strcmp0(name,xml_get_bus_path("VIDEO"))) {
        videoConnected = TRUE;
    }
    dbus_init();
}

void on_name_vanished (GDBusConnection *connection, const gchar *name, gpointer user_data)
{
    char* msg = g_strdup_printf ("Name %s does not exist on %s\n", name,"the session bus");
    print(msg,NULL,INFO);
    g_free(msg);
    if (g_strcmp0(name,xml_get_bus_path("MUSIC"))){
        musicConnected = FALSE;
    } else if (g_strcmp0(name,xml_get_bus_path("VIDEO"))) {
        videoConnected = FALSE;
    }
    dbus_init();
}
