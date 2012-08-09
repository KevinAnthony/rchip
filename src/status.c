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

#include <string.h>
#include <gtk/gtk.h>
#include "status.h"
#include "settings.h"
#include "utils.h"


GtkWidget           *window;
GtkListStore        *status_liststore = NULL;
GMutex              *liststore_lock = NULL;

extern GMutex       *Userpath_lock;
extern char*        Userpath;
extern GAsyncQueue  *gui_async_queue;

void gui_thread_handler(gpointer* NotUsed){
    while(1){
        gpointer *data = g_async_queue_pop (gui_async_queue);
        if (data){
            if (data == THREAD_EXIT) {
                //TODO Save ListStore
                save_liststore_to_file(NULL);
                g_thread_exit (NULL);
            }
            queue_function_data *function_data = (queue_function_data*) data;
            function_data->func(function_data->data);
            g_free(function_data);
        }
    }
}

void init_status_window (gboolean showWindow,char* glade_file) {
    if (liststore_lock == NULL)
        liststore_lock = g_mutex_new();
    GtkBuilder* builder = gtk_builder_new ();
    if (!glade_file)
        gtk_builder_add_from_file (builder, PREFIX "/noside/ui/rchip_status_window.glade", NULL);
    else
        gtk_builder_add_from_file (builder, glade_file, NULL);
    window = GTK_WIDGET (gtk_builder_get_object (builder, "statusWindow"));


    init_status_labels(builder);
    init_xml_labels(builder);
    g_mutex_lock(liststore_lock);
    status_liststore = GTK_LIST_STORE(gtk_builder_get_object(builder, "status_window_liststore"));
    g_mutex_unlock(liststore_lock);
    g_object_unref (G_OBJECT (builder));
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(window_destroyed), NULL);

    queue_function_data* func = g_malloc(sizeof(queue_function_data));
    func->func = *load_liststore_from_file;
    func->data = NULL;
    func->priority = TP_NORMAL;
    g_async_queue_push_sorted(gui_async_queue,(gpointer)func,(GCompareDataFunc)sort_async_queue,NULL);

    if (showWindow)
        gtk_widget_show (window);
}

void init_status_labels(GtkBuilder* builder){
    GtkLabel *label;
    label = GTK_LABEL (gtk_builder_get_object (builder, "version"));
    gtk_label_set_label(label,PACKAGE_VERSION);
    label = GTK_LABEL (gtk_builder_get_object (builder, "program_name"));
    gtk_label_set_label(label,PACKAGE_NAME);
}

void init_xml_labels(GtkBuilder* builder){
    GtkLabel * label;
    char* cLabelM = g_strconcat("Music File:\n",get_file_name_from_path(get_setting_str(MUSIC_XML)),NULL);
    label = GTK_LABEL (gtk_builder_get_object (builder, "music_xml_file"));
    gtk_label_set_label(label,cLabelM);
    g_free(cLabelM);
    char* cLabelV = g_strconcat("Video File:\n",get_file_name_from_path(get_setting_str(VIDEO_XML)),NULL);
    label = GTK_LABEL (gtk_builder_get_object (builder, "video_xml_file"));
    gtk_label_set_label(label,cLabelV);
    g_free(cLabelV);
}

void window_destroyed(GtkWidget *widget, gpointer gdata){
    g_mutex_lock(liststore_lock);
    if ( status_liststore != NULL ) {

        queue_function_data* func = g_malloc(sizeof(queue_function_data));
        func->func = *save_liststore_to_file;
        func->priority = TP_NORMAL;
        func->data = NULL;
        g_async_queue_push_sorted(gui_async_queue,(gpointer)func,(GCompareDataFunc)sort_async_queue,NULL);

    }
    g_mutex_unlock(liststore_lock);
    window=NULL;
}

void close_window(GtkWidget *widget, gpointer gdata){
    show_hide_window();
}

void show_hide_window(){
    if (window != NULL){
        if (gtk_widget_get_visible(window)){
            gtk_widget_hide(window);
        } else {
            gtk_widget_show_all(window);
        }
    } else {
        init_status_window(TRUE,NULL);
    }
}

char* get_file_name_from_path(char* path){
    char* posOfLastSlash=path;
    char* ptr = path;
    for (; *ptr != '\0';ptr++) {
        if (*ptr == '/') {
            posOfLastSlash = ptr;
        }
    }
    posOfLastSlash++;
    return posOfLastSlash;
}

gpointer* insert_into_window(gpointer *data){
    print_data* line = (print_data*) data;
    g_mutex_lock(liststore_lock);
    if (status_liststore != NULL){
        GtkTreeIter insert_iter;
        gtk_list_store_insert(status_liststore,&insert_iter,0);
        gtk_list_store_set(status_liststore,&insert_iter,0,line->thread_id,1,line->time,2,line->event,3,line->data,-1);
    }
    g_mutex_unlock(liststore_lock);
    g_free(line->thread_id);
    g_free(line->time);
    g_free(line->event);
    g_free(line->data);
//    check_reduce_buffer();
}

gpointer* load_liststore_from_file(gpointer *data){
    /*g_mutex_lock(Userpath_lock);
    char* path = g_strdup_printf("%s/.rchip_buffer",Userpath);
    g_mutex_unlock(Userpath_lock);

    FILE *file = fopen(path,"rb");
    g_mutex_lock(status_buffer_lock);
    if (file){
        fseek(file, 0, SEEK_END);
        unsigned long file_len = ftell(file);
        fseek(file, 0, SEEK_SET);

        char* buffer = g_malloc(file_len+1);
        fread(buffer, file_len, 1, file);

        GtkTextIter iter;
        gtk_text_buffer_set_text(status_buffer,buffer,-1);
        fclose(file);
    } else
        gtk_text_buffer_set_text (status_buffer,"",-1);
    g_mutex_unlock(status_buffer_lock);
    g_free(path);
    check_reduce_buffer();*/
    return NULL;
}

gpointer* save_liststore_to_file(gpointer *data){
    /*g_mutex_lock(Userpath_lock);
    char* path = g_strdup_printf("%s/.rchip_buffer",Userpath);
    g_mutex_unlock(Userpath_lock);

    char* buffer = (char*)data;
    FILE* file = fopen(path,"w");
    if (file){
        fprintf(file,buffer);
        fclose(file);
    }
    g_free(path);*/
    return NULL;
}

void check_reduce_buffer(){
    /*
    int buffer_max_size = get_setting_int(STATUS_BUFFER_MAX_SIZE);
    if (buffer_max_size < 0)
        return;
    g_mutex_lock(status_buffer_lock);
    if (gtk_text_buffer_get_line_count(status_buffer) > buffer_max_size){
        GtkTextIter start_iter;
        GtkTextIter end_iter;
        gtk_text_buffer_get_iter_at_line(status_buffer,&start_iter,buffer_max_size);
        gtk_text_buffer_get_end_iter(status_buffer,&end_iter);
        gtk_text_buffer_delete (status_buffer,&start_iter,&end_iter);
    }
    g_mutex_unlock(status_buffer_lock);*/
}
