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

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <glob.h>
#include <dirent.h>
#include "showlist.h"
#include "cmdhandler.h"
#include "settings.h"
#include "utils.h"

extern gboolean program_active;
extern GAsyncQueue *file_async_queue;


void file_thread_handler(gpointer *NotUsed){
    while(program_active){
        gpointer *data = g_async_queue_try_pop (file_async_queue);
        if (data){
            queue_function_data* function_data = (queue_function_data*) data;
            function_data->func(function_data->data);
            g_free(function_data->data);
            g_free(function_data);
        }
    }
}

gpointer* add_file_to_playqueue(gpointer* data){
    //here i really should somehow figure out if it's an anime, a live action, or something else
    // for now, it's all live action
    char* filepath = (char*)data;
    if ((is_valid_extension(filepath))){
        if ((g_strstr_len(filepath,-1,"/English/Live_Action/") != NULL)) {
            send_cmd("ADDS",live_action(filepath),TP_LOW);
        } else if (g_strstr_len(filepath,-1,"/Foreign/Animeted/") != NULL) {
            send_cmd("ADDS",anime(filepath),TP_LOW);
        } else {
            send_cmd("ADDS",other(filepath),TP_LOW);
        }
    }
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
            switch(file_type(newDirFile)){
                case FTDIR:
                    func->func  = *add_file_to_playqueue;
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

char* live_action(char* filepath){
    char* posOfLastSlash=filepath;
    int fnamelen=0;
    int lenOfName=0;
    int lenOfEpsNumber=0;
    int lenOfEpsName=0;
    int totallen=0;
    char* ptr = filepath;
    char* sptr = filepath;
    for (; *ptr != '\0';ptr++) {
        if (*ptr == '/') {
            posOfLastSlash = ptr;
        }
        fnamelen++;
    }
    ptr = posOfLastSlash;
    ptr++;
    sptr=ptr;
    for (int i = 0; i < 3; i++){
        for (; *ptr!='.' ; ptr++){
            if (*ptr == '\0'){
                if (i == 2){ break; }
                #if VERBOSE >= 2
                g_warning("Error: badly formed file name\n");
                g_warning("File Name: %s\n",filepath);
                #endif
                return NULL;
            }
            if (i == 0){ lenOfName++; }
            else if (i == 1){ lenOfEpsNumber++; }
            else if (i == 2){ lenOfEpsName++; }
        }
        ptr++;
    }
    if (*ptr == '\0'){ lenOfEpsName = 0;}
    totallen=fnamelen+lenOfName+lenOfEpsNumber+lenOfEpsName;
    char* retval = g_malloc(totallen+5);
    ptr = retval;
    for (int i = 0; i < lenOfName; i++){
        *ptr++=*sptr++;
    }
    *ptr++='|';
    sptr++;
    for (int i = 0; i < lenOfEpsNumber; i++){
        *ptr++=*sptr++;
    }
    *ptr++='|';
    sptr++;
    for (int i = 0; i < lenOfEpsName; i++){
        *ptr++=*sptr++;
    }
    *ptr++='|';
    sptr=filepath;
    for (int i = 0; i < fnamelen; i++){
        *ptr++=*sptr++;
    }
    *ptr='\0';
    #if VERBOSE >= 4
        g_printf("returnValue of live_action in showlist.c\n%s\n",retval);
    #endif
    return retval;
}

char* anime(char* filepath){
    if (g_strstr_len(filepath,-1,"One_Piece") != NULL) {
        return std_anime(filepath,"One_Piece");
    } else if (g_strstr_len(filepath,-1,"Fairy_Tail") != NULL) {
        return std_anime(filepath,"Fairy_Tail");
    } else {
        return other(filepath);
    }


}

char* other(char* filepath){
    char* posOfLastSlash=filepath;
    int fnamelen=0;
    int lenOfName=0;
    int totallen=0;
    char* ptr = filepath;
    char* sptr = filepath;
    for (; *ptr != '\0';ptr++) {
        if (*ptr == '/') {
            posOfLastSlash = ptr;
        }
        fnamelen++;
    }
    ptr = posOfLastSlash +1;
    for (; *ptr != '\0'; ptr++){
        lenOfName++;
    }
    totallen=fnamelen+lenOfName+5;
    char* retval = g_malloc(totallen+5);
    ptr=retval;
    sptr = posOfLastSlash+1;
    char* other = "Other";
    for (int i = 0; i < 5; i++) {
        *ptr++=*other++;
    }
    *ptr++='|';
    for (int i = 0; i < lenOfName; i++){
        *ptr++=*sptr++;
    }
    *ptr++='|';
    *ptr++='|';
    sptr=filepath;
    for (int i = 0; i < fnamelen; i++){
        *ptr++=*sptr++;
    }
    *ptr='\0';
    return retval;

}

char* std_anime(char* filepath,char* name){
    char* posOfLastSlash=filepath;
    int fnamelen=0;
    int lenOfName=strlen(name);
    int lenOfEpsNumber=0;
    int lenOfSubgroup=0;
    int totallen=0;
    char* ptr = filepath;
    char* sptr = filepath;
    for (; *ptr != '\0';ptr++) {
        if (*ptr == '/') {
            posOfLastSlash = ptr;
        }
        fnamelen++;
    }
    ptr = posOfLastSlash+1;
    ptr = ptr+lenOfName+1;
    sptr=ptr;
    for (; *ptr!='_' ; ptr++){
        if (*ptr == '\0'){
            #if VERBOSE >= 2
            printf("Error: badly formed file name\n");
            printf("File Name: %s\n",filepath);
            #endif
            return NULL;
             }
        lenOfEpsNumber++;
    }
    ptr++;
    ptr++;
    for (; *ptr!=']' ; ptr++){
           if (*ptr == '\0'){
               #if VERBOSE >= 2
               printf("Error: badly formed file name\n");
               printf("File Name: %s\n",filepath);
               #endif
               return NULL;
           }
           lenOfSubgroup++;
       }

    if (*ptr == '\0'){ lenOfSubgroup = 0;}
    totallen=fnamelen+lenOfName+lenOfEpsNumber+lenOfSubgroup;
    char* retval = g_malloc(totallen+5);
    ptr = retval;
    sptr=name;
    for (int i = 0; i < lenOfName; i++){
        *ptr++=*sptr++;
    }
    *ptr++='|';
    sptr=posOfLastSlash+lenOfName+2;
    for (int i = 0; i < lenOfEpsNumber; i++){
        *ptr++=*sptr++;
    }
    *ptr++='|';
    while (*sptr++ != '['){ lenOfSubgroup--; }
    lenOfSubgroup++;
    for (int i = 0; i < lenOfSubgroup; i++){
        *ptr++=*sptr++;
    }
    *ptr++='|';
    sptr=filepath;
    for (int i = 0; i < fnamelen; i++){
              *ptr++=*sptr++;
    }
    *ptr='\0';
    return retval;
}
