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

#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include <glib/gprintf.h>
#include "utils.h"

gboolean            program_active = FALSE;

GAsyncQueue         *network_async_queue = NULL;
GAsyncQueue         *file_async_queue = NULL;
GAsyncQueue         *gui_async_queue = NULL;

gchar* replace_str (const gchar *src,const gchar *find,const gchar *replace){
    gchar* retval = g_strdup(src);
    gchar* ptr = NULL;
    ptr = g_strstr_len(retval,-1,find); 
    if (ptr != NULL){
        gchar* after_find = replace_str(ptr+strlen(find),find,replace);
        gchar* before_find = g_strndup(retval,ptr-retval);
        gchar* temp = g_strconcat(before_find,replace,after_find,NULL);
        g_free(retval);
        retval = g_strdup(temp);
        g_free(before_find);
        g_free(temp);
    }
    return retval;
}

void init_hostname ( void ){
    Hosts = malloc(sizeof(hostname*));
    Hosts->add = *add;
    Hosts->delete = *delete;
    Hosts->find = *find;
    Hosts->data=NULL;
}

void add (char* data){
    if (!Hosts->find(data)){
        hostname_node* new_node = malloc(sizeof(hostname_node*));
        new_node->hostname = g_strdup(data);
        new_node->next = NULL;
        if (Hosts->data == NULL){
            Hosts->data = new_node;
            return;
        }
        hostname_node* hostname_p = Hosts->data;
        while (hostname_p->next != NULL){next_hostname(hostname_p);}
        hostname_p->next = new_node;
    }
}

void delete (char* data){
    if (Hosts->data == NULL){
        return;
    }
    hostname_node* cur = Hosts->data;
    hostname_node* prev = NULL;
    while (cur != NULL){
        if (g_strcmp0(cur->hostname,data) == 0){
            if (cur == Hosts->data){
                Hosts->data=cur->next;
                g_free(cur->hostname);
                g_free(cur);
                cur = Hosts->data;
            } else {
                prev->next = cur->next;
                g_free(cur->hostname);
                g_free(cur);
                cur = prev->next;
            }
        }
        if (prev == NULL){
            prev = Hosts->data;
        } else {
            next_hostname(prev);
        }
        next_hostname(cur);
    }
}

int find (char* token){
    
    hostname_node* hostname_p = Hosts->data;
    for_each_hostname(hostname_p){
        if (g_strcmp0(hostname_p->hostname, token) == 0){
            return 1;
        }
    }
    return 0;
}

gboolean is_valid_extension(const gchar* filename){
    return g_str_has_suffix (filename,"mkv") || g_str_has_suffix (filename,"avi") || g_str_has_suffix (filename,"mp4");
}

gboolean queue_init( void ){
    network_async_queue = g_async_queue_new();
    if (!network_async_queue)
        return FALSE;
    file_async_queue = g_async_queue_new();
    if (!file_async_queue)
        return FALSE;
    gui_async_queue = g_async_queue_new();
    if (!gui_async_queue)
        return FALSE;
    program_active = TRUE;
    return TRUE;
}
