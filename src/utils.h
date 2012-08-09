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
#ifndef UTILS_H
#define UTILS_H

#define next_hostname(hn) \
    hn=hn->next
#define for_each_hostname(hn) \
    for (hn=Hosts->data; hn != NULL; next_hostname(hn))

#define             THREAD_EXIT                 0x0051


typedef enum {
    NOTHING,
    ERROR,
    WARNING,
    INFO,
    DEBUG
} log_level;

typedef enum {
    TP_LOW,
    TP_NORMAL,
    TP_HIGH,
    TP_CRITICAL
} thread_priority;

typedef struct PlayingInfoMusic {
    char    *Artist;
    char    *Album;
    char    *Song;
    int     Elapised_time;
    int     Duration;
    int     isPlaying;
} playing_info_music;

typedef struct HostNamesNodeStruct {
    char* hostname;
    struct HostNamesNodeStruct* next;
} hostname_node;

typedef struct HostNameStruct{
    hostname_node *data;
    void (*add)(char *);
    void (*delete)(char *);
    int (*find)(char *);
} hostname;

typedef struct QueueFunctionData{
    gpointer* (*func)(gpointer*);
    gpointer* data;
    thread_priority priority;
} queue_function_data;

typedef struct _print_data{
    char*   thread_id;
    char*   time;
    char*   event;
    char*   data;
} print_data;

hostname*           get_host_head               ( void );
gchar*              replace_str                 ( const gchar* , const gchar*, const gchar* );
void                add                         ( char* );
void                delete                      ( char* );
int                 find                        ( char* );
void                init_hostname               ( void );
gboolean            is_valid_extension          ( const gchar* );
gboolean            queue_init                  ( void );
gint                sort_async_queue            ( gconstpointer a, gconstpointer b, gpointer user_data );
void                print                       ( const gchar*, const gchar*, int);

#endif
