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
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <config.h>

#include "settings.h"
#include "status.h"
#include "utils.h"
#include <stdlib.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <glib.h>
#include <gio/gio.h>
GSettings *settings = NULL;

char* get_setting_str( gchar* keyname) {
    /*
     * we search based on keyname, and return the value
     */
    print("Trying to get Setting for Value",keyname,DEBUG);
    char* valueStr = NULL;
    if (settings != NULL) {
        valueStr = g_settings_get_string (settings,keyname);
    } else {
        return NULL;
    }
    print("Value for key",valueStr,DEBUG);
    if (valueStr == NULL) {
        print("No Value",keyname,WARNING);
        return NULL;
    }
    return valueStr;
}

gboolean set_setting_str( gchar* keyname, gchar* settingValue ){
    print("Setting Value",g_strdup_printf("%s->%s",keyname,settingValue),INFO);
    return g_settings_set_string(settings,keyname,settingValue);
}

int get_setting_int( gchar* keyname) {
    /*
     * we search based on keyname, and return the value
     */
    print("Trying to get Setting for Value",keyname,DEBUG);
    int valueInt = -1;
    valueInt = g_settings_get_int(settings,keyname);
    char* tempitoa = g_strdup_printf("%i",valueInt);
    print("Value for key",tempitoa,DEBUG);
    g_free(tempitoa);
    if (valueInt == -1) {
        print("No Value",keyname,WARNING);
        return -1;
    }
    return valueInt;
}

gboolean set_setting_int( gchar* keyname, int settingValue ){
    print("Setting Value",g_strdup_printf("%s->%i",keyname,settingValue),INFO);
    return g_settings_set_int(settings,keyname,settingValue);
}

void setting_changed( GSettings *settings, gchar *key, gpointer user_data){
    print("Setting Changed",key,DEBUG);
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
