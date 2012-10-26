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

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <glob.h>
#include <dirent.h>
#include <unistd.h>
#include <regex.h>
#include "showlist.h"
#include "cmdhandler.h"
#include "settings.h"
#include "utils.h"

extern GAsyncQueue *file_async_queue;


void file_thread_handler(gpointer *NotUsed){
    register_thread("Disk I/O Thread");
    while(1){
        gpointer *data = g_async_queue_pop (file_async_queue);
        if (data){
            if (data == THREAD_EXIT){
                unregister_thread();
                g_thread_exit (NULL);
            }
            queue_function_data *function_data = (queue_function_data*) data;
            function_data->func(function_data->data);
            g_free(function_data);
        }
    }
}

gpointer* add_file_to_playqueue(gpointer* data){
    char* filepath = (char*)data;
    if ((is_valid_extension(filepath)))
        send_cmd("ADDS",standard_name(filepath),TP_LOW);
    return NULL;
}

gpointer* add_folder_to_playqueue(gpointer* data){
    /* this sets threw each folder recursivly and adds all file to playqueue (although it's really a list)*/
    DIR *dp;
    struct dirent *ep;
    char *newDirFile;
    char* dirFile = (char*) data;
    if ((dp=opendir(dirFile))==NULL) {
        dirFile = replace_str(dirFile,get_setting_str(VIDEO_ROOT),"/mnt/raid/");
        add_file_to_playqueue((gpointer*)dirFile);
    } else {
        while((ep=readdir(dp))) {
            queue_function_data* func = g_malloc(sizeof(queue_function_data));
            if (!strcmp(ep->d_name,".") || !strcmp(ep->d_name, "..")) { continue; }
            newDirFile = g_strdup_printf("%s/%s", dirFile, ep->d_name);
            printf("Directory: %s",newDirFile);
            switch(file_type(newDirFile)){
                case FTDIR:
                    func->func  = *add_folder_to_playqueue;
                    func->data = g_strdup(newDirFile);
                    func->priority = TP_NORMAL;
                    g_async_queue_push_sorted(file_async_queue,(gpointer)func,(GCompareDataFunc)sort_async_queue,NULL);
                    break;
                case FTFILE:
                    newDirFile = replace_str(newDirFile,get_setting_str(VIDEO_ROOT),"/mnt/raid/");
                    func->func  = *add_file_to_playqueue;
                    func->data = g_strdup(newDirFile);
                    func->priority = TP_NORMAL;
                    g_async_queue_push_sorted(file_async_queue,(gpointer)func,(GCompareDataFunc)sort_async_queue,NULL);
                    break;
                case FTDONOTPROC:
                    break;
            }
            g_free(newDirFile);
        }
        closedir(dp);
    }
    return NULL;
}

int file_type(char *name){
    /* gets the file type from the path */
    DIR *dp;
    int ret = FTFILE;
    if (!((dp=opendir(name))==NULL)){
        ret = FTDIR;
        closedir(dp);
    }
    return ret;
}

char* standard_name (char* filepath){
    char* original_file_path = g_strdup(filepath);
    char* result = strtok(original_file_path,"/");
    char* filename = NULL;
    regex_t regex;

    while (result != NULL){
        filename = result;
        result = strtok(NULL,"/");
    }
    g_free(original_file_path);

    if (filename == NULL)
        return unstandard_name(filepath);

    char* show_name = g_strdup(strtok(filename,"."));
    if ( show_name == NULL )
        return unstandard_name(filepath);

    char* episode_number =  g_strdup(strtok(NULL,"."));
    if ( episode_number == NULL )
        return unstandard_name(filepath);

    char* episode_name =  g_strdup(strtok(NULL,"."));
    if (episode_name == NULL)
        return unstandard_name(filepath);

    char* retval = NULL;

    if (regcomp(&regex,"S[0-9]{2}E[0-9]{2}",REG_EXTENDED))
        g_error("Problem Setting up Regular Expression");
    else {
        int regi = regexec(&regex, filepath, 0, NULL, 0);
        if (!regi )
            retval = g_strdup_printf("%s|%s|%s|%s",show_name,episode_number,episode_name,filepath);
        else if ( regi == REG_NOMATCH )
            retval = g_strdup_printf("%s|%s|%s %s|%s",show_name,episode_number,episode_number,show_name,filepath);
    }
    g_free(show_name);
    g_free(episode_number);
    g_free(episode_name);
    regfree(&regex);
    return retval;
}

char* unstandard_name(char* filepath){
    char* original_file_path = g_strdup(filepath);
    char* result = strtok(original_file_path,"/");
    char* filename = NULL;

    while (result != NULL){
        filename = result;
        result = strtok(NULL,"/");
    }
    g_free(original_file_path);
    return g_strdup_printf("Other|%s|%s|%s",filename,filename,filepath);
}
