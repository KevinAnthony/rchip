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

#ifndef _WIN32
	#include 	"dbus.h"
	#include	"utils.h"
	#include	"notifications.h"
	#include	<string.h>
	#include	<gio/gio.h>	
	#include	<glib.h>
	#include	<glib/gprintf.h>
	
	/*we set the dbus timeout to 3 seconds*/
	#define		DBUS_TIMEOUT	3000
	/* two proxies and the connection*/
	/* 
         * TODO:replace these two with simply a generic proxy connection
	 * maybe called dbusConn taken in from the XML from cmdHandler or more likly a gsetting
	 */
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
		/* 
		 * TODO:
		 * replace this whole new sync #if #else with a single connection, set in gsettings
		 * we need to implemtn a callback so if the proxy is changed, we reset it
		 */
		#ifdef RHYTHMBOX
		shell = g_dbus_proxy_new_sync(conn, G_DBUS_CALL_FLAGS_NONE,NULL,"org.gnome.Rhythmbox",
						     "/org/gnome/Rhythmbox/Shell",
						      "org.gnome.Rhythmbox.Shell",NULL,&error);
		if (error != NULL) {
			#if VERBOSE >= 1
			g_error("Somethings wrong:dbus_g_proxy_new_for_name_owner(SHELL)\n");
			g_error("%s\n",error->message);
			#endif
			conn =NULL;
			return FALSE;
		}
		player = g_dbus_proxy_new_sync(conn, G_DBUS_CALL_FLAGS_NONE,NULL,"org.gnome.Rhythmbox",
						     "/org/gnome/Rhythmbox/Player",
						      "org.gnome.Rhythmbox.Player",NULL,&error);     
		if (error != NULL) {
			#if VERBOSE >= 1
			g_error("Somethings wrong:dbus_g_proxy_new_for_name_owner(PLAYER)\n");
			g_error("%s\n",error->message);
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
			g_error("Somethings wrong:dbus_g_proxy_new_for_name_owner(PlayerEngine)\n");
			g_error("%s\n",error->message);
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
			g_error("Somethings wrong:dbus_g_proxy_new_for_name_owner(PlaybackController)\n");
			g_error("%s\n",error->message);
			#endif
			conn =NULL;
			return FALSE;
		}

		#endif
		return TRUE;
	}
	/* TODO:replace this with a single call sync*/
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
			g_error("Error with getPlaying: %s\n",error->message);
			#endif
			return FALSE;
		 } else {
			return TRUE;
		}
	}
	/* TODO:replace this witha single call sync */
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
			g_error("Error with getPlaying: %s\n",error->message);
			#endif
			return FALSE;
		 } else {
			return TRUE;
		}
	}
	
	/* get playing info from music player */
	struct playing_info_music dbus_get_playing_info_music() {
		struct playing_info_music pInfo;
		GError *error = NULL;
		/* Make sure we are connected to dbus */
		if (dbus_is_connected(TRUE)) {
			/* 
			 * dbus and rhythmbox are raticlly diffrent here
			 * I don't know if we can combind this into one command, but i should look into it
			 */
			#ifdef RHYTHMBOX	
			gboolean playing;
			/* Check to see if the player is playing, if it's not, no point going any further*/
			GVariant* results =g_dbus_proxy_call_sync(player,"getPlaying",NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
			if (error != NULL) {
				#if VERBOSE >= 1
				g_error("Error with getPlaying: %s\n",error->message);
				#endif
			}
			playing = g_variant_get_boolean(g_variant_get_child_value(results,0));
			if (playing){
				/* Get the current playing URI so we can get it's properties*/
				results =g_dbus_proxy_call_sync(player,"getPlayingUri",NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
				if (error == NULL) {
					/* TODO: we need to check that results <> NULL */
					const char* uri = g_variant_get_string(g_variant_get_child_value(results,0),NULL);
					/* using the URI above, we need to the the songPorperties */
					results =g_dbus_proxy_call_sync(shell,"getSongProperties",g_variant_new ("(s)",uri),G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
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
					pInfo.Artist = g_malloc(strlen(tempVar)+1);
					strcpy(pInfo.Artist,tempVar);
					g_free(tempVar);

					tempVar=g_malloc(8192);
					g_variant_lookup(dict,"album","s",&tempVar);
					pInfo.Album = g_malloc(strlen(tempVar)+1);
					strcpy(pInfo.Album,tempVar);

					g_free(tempVar);
					tempVar=g_malloc(8192);
					g_variant_lookup(dict,"title","s",&tempVar);
					pInfo.Song = g_malloc(strlen(tempVar)+1);
					strcpy(pInfo.Song,tempVar);

					g_free(tempVar);
					g_variant_lookup(dict,"duration","d",&doubleValue);
					pInfo.Duration = doubleValue;
					pInfo.isPlaying=playing;
					/* we get the elaped time playing*/
					results =g_dbus_proxy_call_sync(player,"getElapsed",NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
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
			GVariant* result = g_dbus_proxy_call_sync(playerEngine,"GetCurrentState",NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
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
					result =g_dbus_proxy_call_sync(playerEngine,"GetCurrentTrack",NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
					GVariant* dict = g_variant_get_child_value(result,0);
					double doubleValue;
					/* this works like like loopup above*/
					char* tempVar = g_malloc(8192);
					g_variant_lookup(dict,"artist","s",&tempVar);
					pInfo.Artist = g_malloc(strlen(tempVar)+1);
					strcpy(pInfo.Artist,tempVar);
					g_free(tempVar);

					tempVar=g_malloc(8192);
					g_variant_lookup(dict,"album","s",&tempVar);
					pInfo.Album = g_malloc(strlen(tempVar)+1);
					strcpy(pInfo.Album,tempVar);
					g_free(tempVar);

					tempVar=g_malloc(8192);
					g_variant_lookup(dict,"name","s",&tempVar);
					pInfo.Song = g_malloc(strlen(tempVar)+1);
					strcpy(pInfo.Song,tempVar);
					g_free(tempVar);

					g_variant_lookup(dict,"length","d",&doubleValue);
					pInfo.Duration = doubleValue;
					/* 
					 * this is tricky, c and t are current posistion and total length in bytes 
					 * we use basic algebra to get the position in secounds using cross-multiply and divide 
					 */
					result = g_dbus_proxy_call_sync(playerEngine,"GetPosition",NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
					unsigned int c = g_variant_get_uint32(g_variant_get_child_value(result,0));
					result = g_dbus_proxy_call_sync(playerEngine,"GetLength",NULL,G_DBUS_CALL_FLAGS_NONE,DBUS_TIMEOUT,NULL,&error);
					unsigned int t = g_variant_get_uint32(g_variant_get_child_value(result,0));
					pInfo.Elapised_time = (c*pInfo.Duration)/t;
				}
			} else {
				#if VERBOSE >= 1
				g_error("ERROR:%s\n",error->message);
				#endif
			}
			#endif
		}
		return pInfo;	
	}
	/* This outputs the information in human readable */
	void print_playing_info_music(const struct playing_info_music pInfo){
		#if VERBOSE >= 4
		g_printf("Artist\t\t%s\nAlbum\t\t%s\nSong\t\t%s\nElapised Time\t%i\nDuration\t%i\nIs Playing\t%i\n\n",pInfo.Artist,pInfo.Album,pInfo.Song,pInfo.Elapised_time,pInfo.Duration,pInfo.isPlaying);
		#endif
	}
#endif
