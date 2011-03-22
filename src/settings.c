/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
*
*	rchip, Remote Controlled Home Integration Program
*	Copyright (C) 2011 <Kevin@NoSideRacing.com>
*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.
*
*	This program is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with this program. If not, see <http://www.gnu.org/licenses/>.
*
*/

#include <config.h>

#include "settings.h"
#include <glib.h>
#include <glib/gprintf.h>
#include <glib.h>
#include <gio/gio.h>
GSettings *settings;

char* get_setting_str( gchar* keyname) {
	/*
	 * we search based on keyname, and return the value
	 */
	#if VERBOSE >= 3
	printf("Trying to get Setting for Value:%s\n",keyname);
	#endif
	char* valueStr = NULL;
	valueStr = g_settings_get_string (settings,keyname);
	#if VERBOSE >= 4
	printf("Value for key is:%s\n",valueStr);
	#endif
	if (valueStr == NULL) {
	 		#if VERBOSE >= 2 	
		g_error("Error: No Value for %s",keyname);
		#endif
		return NULL;
	}
	return valueStr;
}

gboolean set_setting_str( gchar* keyname, gchar* settingValue ){
	#if VERBOSE >= 3
	printf("Trying to set Setting with value:%s--%s\n",keyname,settingValue);
	#endif 
	return g_settings_set_string(settings,keyname,settingValue);
}

int get_setting_int( gchar* keyname) {
	/*
	 * we search based on keyname, and return the value
	 */
	#if VERBOSE >= 3
	printf("Trying to get Setting for Value:%s\n",keyname);
	#endif
	int valueInt = 0;
	valueInt = g_settings_get_int(settings,keyname);
	#if VERBOSE >= 4
	printf("Value for key is:%d\n",valueInt);
	#endif
	if (valueInt == 0) {
		#if VERBOSE >= 2
		g_error("Error: No Value for %s",keyname);
		#endif
		return -1;
	}
	return valueInt;
}

gboolean set_setting_int( gchar* keyname, int settingValue ){
	#if VERBOSE >= 3
	printf("Trying to set Setting with value:%s--%d\n",keyname,settingValue);
	#endif
	return g_settings_set_int(settings,keyname,settingValue);
}

void setting_changed( GSettings *settings, gchar *key, gpointer user_data){
	g_printf("Setting %s Changed",key);
}


void settings_unref(){
	if (settings){
		g_object_unref(settings);
	}
}

gboolean settings_init(){
	settings = g_settings_new("apps.noside.rchip.settings");
	/* catch emitted signal "changed" here*/
	g_signal_connect(settings,"changed", G_CALLBACK(setting_changed),NULL);
	return TRUE;
}
