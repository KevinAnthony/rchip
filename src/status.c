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
GtkTextBuffer       *status_buffer = NULL;

extern GMutex       *Userpath_lock;
extern char*        Userpath;
extern GAsyncQueue  *gui_async_queue;

void gui_thread_handler(gpointer* NotUsed){
    while(1){
        gpointer *data = g_async_queue_pop (gui_async_queue);
        if (data){
            if (data == THREAD_EXIT) {
                GtkTextIter start_iter;
                GtkTextIter end_iter;
                gtk_text_buffer_get_start_iter(status_buffer,&start_iter);
                gtk_text_buffer_get_end_iter(status_buffer,&end_iter);
                write_to_buffer(g_strdup(gtk_text_buffer_get_text ( status_buffer, &start_iter, &end_iter, TRUE)));
                g_thread_exit (NULL);
            }
            queue_function_data *function_data = (queue_function_data*) data;
            function_data->func(function_data->data);
            g_free(function_data);
        }
    }
}

void init_status_window (gboolean showWindow) {
    GtkBuilder* builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, PREFIX "/noside/ui/rchip_status_window.glade", NULL);
    window = GTK_WIDGET (gtk_builder_get_object (builder, "statusWindow"));


    init_status_labels(builder);
    init_xml_labels(builder);
    status_buffer = GTK_TEXT_BUFFER(gtk_builder_get_object(builder, "status_text_buffer"));
    g_object_unref (G_OBJECT (builder));
    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(window_destroyed), NULL);

    queue_function_data* func = g_malloc(sizeof(queue_function_data));
    func->func = *read_to_buffer;
    func->priority = TP_NORMAL;

    func->data = NULL;
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
    if ( status_buffer != NULL ) {
        GtkTextIter start_iter;
        GtkTextIter end_iter;
        gtk_text_buffer_get_start_iter(status_buffer,&start_iter);
        gtk_text_buffer_get_end_iter(status_buffer,&end_iter);

        queue_function_data* func = g_malloc(sizeof(queue_function_data));
        func->func = *write_to_buffer;
        func->priority = TP_NORMAL;
        func->data = g_strdup(gtk_text_buffer_get_text ( status_buffer, &start_iter, &end_iter, TRUE));
        g_async_queue_push_sorted(gui_async_queue,(gpointer)func,(GCompareDataFunc)sort_async_queue,NULL);

    }
    printf("Window Destroyed\n");
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
        init_status_window(TRUE);
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
    char* line = (char*) data;
    if (status_buffer != NULL){
        GtkTextIter iter;
        gtk_text_buffer_get_start_iter(status_buffer,&iter);
        gtk_text_buffer_insert (status_buffer,&iter,line,-1);
    }

}

gpointer* read_to_buffer(gpointer *data){
    g_mutex_lock(Userpath_lock);
    char* path = g_strdup_printf("%s/.rchip_buffer",Userpath);
    g_mutex_unlock(Userpath_lock);

    FILE *file = fopen(path,"rb");
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
    g_free(path);
    return NULL;
}

gpointer* write_to_buffer(gpointer *data){
    g_mutex_lock(Userpath_lock);
    char* path = g_strdup_printf("%s/.rchip_buffer",Userpath);
    g_mutex_unlock(Userpath_lock);

    char* buffer = (char*)data;
    FILE* file = fopen(path,"w");
    if (file){
        fprintf(file,buffer);
        fclose(file);
    }
    g_free(path);
    return NULL;
}
