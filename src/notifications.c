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

#include    "notifications.h"
#include    <glib.h>
#include    <glib/gprintf.h>
#include    <string.h>



/*
 * pass notifcations to host devices
 *
 * Ticker String is the text that is displayed on the menu bar when the message is first receaved
 * Notification title is the bold, usally short, title of the notification when you pull down the notification bar
 * Notification text is the text unter the title, more descriptive
 *
 */
gboolean set_notification(char* tickerString,char* notificationTitle,char* notificationText) {
    #ifdef _SQL
    /*
     * nelem is the number of elements in the array
     * size is the size in bytes of each element
     * base is a pointer to the first byte of the first element of the array
     */
    size_t nelem;
    size_t size;
    char* base;
    size=get_size();
    nelem=get_nelem();
    base = g_malloc(nelem*size);
    get_active_devices(base,size,nelem);
    /* we allocate and build the message, when passed to a device they are pipe | delineated */
    char* msg = g_strdup_printf("%s|%s|%s",tickerString,notificationTitle,notificationText);
    /* TMSG command tells the device to display the notification*/
    char* cmd = "TMSG";
    char* recipt = g_malloc(size);
    char* name = NULL;
    /* we get the current hostname of the PC this program is running on */
    struct utsname uts;
    uname( &uts );
    name = g_malloc(strlen(uts.nodename)+2);
    int len = strlen(uts.nodename);
    char* p;
    p=name;
    for (int i = 0; i<len; i++){*p++ = uts.nodename[i];}
    /* For each element in the array, we build a query, and send it to the device */
    for (int i = 0; i < nelem; i++) {
        char* elem = base+(i*size);
        g_strlcpy(recipt,elem,size);
        char* query = g_strdup_printf("Insert into cmdQueue (command,cmdText,source_hostname,dest_hostname) values (\"%s\",\"%s\",\"%s\",\"%s\")",cmd,msg,name,recipt);
        sql_exec_quary(query);
        g_free(query);
    }
    /* And we free everything */
    g_free(recipt);
    g_free(msg);
    g_free(name);
    g_free(base);
    return TRUE;
    #else
    return FALSE;
    #endif
}
