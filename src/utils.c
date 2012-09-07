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

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include <glib/gprintf.h>
#include "status.h"
#include "utils.h"


GAsyncQueue         *network_async_queue = NULL;
GAsyncQueue         *file_async_queue = NULL;
GAsyncQueue         *gui_async_queue = NULL;

GThread             *network_thread;
GThread             *file_thread;
GThread             *gui_thread;

GMutex              *Hosts_lock = NULL;
hostname*           Hosts = NULL;

GMutex              *Userpath_lock = NULL;
char*               Userpath = NULL;

GMutex              *Threads_lock = NULL;
running_threads*    Threads = NULL;

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
    Hosts = malloc(sizeof(hostname));
    Hosts->add = *add;
    Hosts->delete = *delete;
    Hosts->find = *find;
    Hosts->data=NULL;
}

void add (char* data){
    if (!Hosts->find(data)){
        hostname_node* new_node = malloc(sizeof(hostname_node));
        new_node->hostname = g_strdup(data);
        new_node->next = NULL;
        if (Hosts->data == NULL){
            Hosts->data = new_node;
            return;
        }
        hostname_node* hostname_p = Hosts->data;
        while (hostname_p->next != NULL){next_node(hostname_p);}
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
            next_node(prev);
        }
        next_node(cur);
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
    return TRUE;
}

gint sort_async_queue(gconstpointer a, gconstpointer b, gpointer user_data){
    gint32 id1;
    gint32 id2;

    id1 = ((queue_function_data*) a)->priority;
    id2 = ((queue_function_data*) b)->priority;

    return (id1 > id2 ? +1 : id1 == id2 ? 0 : -1);
}

void print (const gchar* event, const char* data,int verbosity){
    if (VERBOSE >= verbosity){
        time_t timer;
        char* timestring[64];
        struct tm datetime;
        time_t now;
        now = time(NULL);
        datetime = *(localtime(&now));
        strftime(timestring,64,"%m-%d-%y %H:%M:%S",&datetime);
        char* threadID = thread_name(g_thread_self());

        char* event_output = g_strdup(event != NULL ? event : " " );
        char* data_output = g_strdup(data != NULL ? data : " " );

        printf("%s\t%s\t%s\t%s\n",threadID,timestring,event_output,data_output);

        print_data* pdata = g_malloc(sizeof(print_data));
        pdata->thread_id = g_strdup(threadID);
        pdata->time = g_strdup(timestring);
        pdata->event = g_strdup(event_output);
        pdata->data = g_strdup(data_output);
        queue_function_data* func = g_malloc(sizeof(queue_function_data));
        func->func = *insert_into_window;
        func->data = (gpointer)pdata;
        func->priority = TP_LOW;
        g_async_queue_push_sorted(gui_async_queue,(gpointer)func,(GCompareDataFunc)sort_async_queue,NULL);

        g_free(event_output);
        g_free(data_output);
    }
}

void register_thread(char* name) {
    if (Threads_lock == NULL)
        Threads_lock = g_mutex_new();
    g_mutex_lock(Threads_lock);
    GThread* self = g_thread_self();
    running_threads* thread;
    for_each_thread(thread){
        if (thread->thread_id == self){
            g_free(thread->thread_name);
            thread->thread_name = g_strdup(name);
            g_mutex_unlock(Threads_lock);
            return;
        }
    }
    thread = g_malloc(sizeof(running_threads));
    thread->thread_id = self;
    thread->thread_name = g_strdup(name);
    thread->next = NULL;
    if (Threads == NULL){
        Threads = thread;
        g_mutex_unlock(Threads_lock);
        return;
    }
    running_threads* cur_thread = Threads;
    while (cur_thread->next != NULL)
        next_node(cur_thread);
    cur_thread->next = thread;
    g_mutex_unlock(Threads_lock);
}

void unregister_thread(){
    assert(Threads);
    g_mutex_lock(Threads_lock);
    GThread* self = g_thread_self();
    if (Threads->next == NULL){
        g_free(Threads->thread_name);
        Threads = NULL;
        return;
    }
    running_threads* thread = Threads->next;
    running_threads* last_thread = Threads;
    for_each_thread(thread){
        if (thread->thread_id == self){
            g_free(thread->thread_name);
            last_thread->next = thread->next;
        }
        next_node(last_thread);
    }
    g_mutex_unlock(Threads_lock);
}

char* thread_name(GThread* self){
    assert(Threads);
    running_threads* thread = Threads;
    for_each_thread(thread)
        if (thread->thread_id == self)
            return thread->thread_name;
    return "ERROR";
}
            
