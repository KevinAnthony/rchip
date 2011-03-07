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
#include <glib.h>
#include <glib/gprintf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#endif
#include "tray.h"
#include "showlist.h"
#include "utils.h"
#include "settings.h"
#include "status.h"

void start_tray(){
	#ifdef GTK3
		/* set up a new GtkApplication and register callback*/
		GtkApplication *app;
		gint status;
		app = gtk_application_new("org.noside.rchip", 0);
		g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
		g_application_register(G_APPLICATION(app),NULL,NULL);
		/*
		 * we check to see if this application is primary(or not remote) and 
		 * run it if it's primary otherwise, we let it die
		 * because we would have nested gtk_main loops, which causes quit to
		 * not function properly
		 */
		if (!(g_application_get_is_remote(G_APPLICATION(app)))){
			status = g_application_run(G_APPLICATION (app),0,NULL);
		}
		g_object_unref(app);
	#else
		gtk_main ();
	#endif
}
#if GTK_MAJOR_VERSION >= 3 
void activate (GtkApplication *app)
{
	#if VERBOSE >= 3
	g_printf("Application Activated\n");
	#endif
	gtk_main();
}
#endif
void tray_click(GtkStatusIcon *status_icon,gpointer user_data)
{
	/* on primary click(default:left) we either show or hide the status window */
	#if VERBOSE >= 3
	g_printf("clicked on Icon\n");
	#endif
	show_hide_window();
}

void tray_menu(GtkStatusIcon *status_icon, guint button, guint activate_time, gpointer user_data)
{
	/* on secondary clicks(default:right) we show the menu */
	#if VERBOSE >= 3
	g_printf("clicked on menu\n");
	#endif
	GtkWidget *tray_menu = create_tray_menu(status_icon);
	gtk_menu_popup (GTK_MENU (tray_menu), NULL, NULL,gtk_status_icon_position_menu,status_icon,button,activate_time);
}

void add_files(GtkWidget *widget, gpointer gdata){
	/*Add Show files to remote watch list*/
 	#if VERBOSE >= 3
	g_printf("addShows Clicked\n");
	#endif
	GtkWidget *dialog;
	dialog = gtk_file_chooser_dialog_new ("Open File",
				NULL,
				GTK_FILE_CHOOSER_ACTION_OPEN,
				GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				NULL);
	/* default to the VIDEO ROOT path */
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER (dialog),get_setting(VIDEO_ROOT));	
	gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (dialog),TRUE);
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT){
		char *filename;
		GSList* filelist = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER (dialog));
		GSList* node = filelist;
		while (node != NULL){
			filename = (char*)node->data;
			/* 
			 * files come back looking like /path/to\file 
			 * or in windows c:\path\to\file, sql can't handle that, as \ is an escape char
			 * so we set it default to c:/path/to/file and set it back to c:\path\to\file when it plays
			 */
			#ifdef _WIN32
			char* p = filename;
			while (*p++ != '\0'){ if (*p == '\\') { *p = '/';} }
			#endif
			#if VERBOSE >= 3
			g_printf("Adding Filename:%s\n",filename);
			#endif
			/* because the user may not use the same machine to watch as they did to add, we replace the local path with a server absoulte path*/
			filename = replace_str(filename,get_setting(VIDEO_ROOT),"/mnt/raid/");
			add_file_to_playqueue(filename);
			node=node->next;
		}
		g_slist_free(filelist);
	}
	gtk_widget_destroy (dialog);

}

void add_folders(GtkWidget *widget, gpointer gdata){
	/* this works the same as the add_files above, but recursivly adds folders instead of single or multiple files in 1 folder */
	#if VERBOSE >= 3
	g_printf("addFolders Clicked\n");
	#endif
	GtkWidget *dialog;
	dialog = gtk_file_chooser_dialog_new ("Open File",
				NULL,
				GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
				GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				NULL);
	gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER (dialog),get_setting(VIDEO_ROOT));
	gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (dialog),TRUE);
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT){
		char *filename;
		GSList* filelist = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER (dialog));
		GSList* node = filelist;
		while (node != NULL){
			filename = (char*)node->data;
			#ifdef _WIN32
			char* p = filename;
			while (*p++ != '\0'){ if (*p == '\\') { *p = '/';} }
			#endif
			#if VERBOSE >= 3
			g_printf("Adding Filename%s\n",filename);
			#endif
			filename = replace_str(filename,get_setting(VIDEO_ROOT),"/mnt/raid/");
			add_folder_to_playqueue(filename);
			node=node->next;
		} 
		g_slist_free(filelist);
		//g_free (filename);
	}
	gtk_widget_destroy (dialog);

}

void add_folder_to_playqueue(char *dirFile){
	/* this sets threw each folder recursivly and adds all file to playqueue (although it's really a list)*/
	DIR *dp;
	struct dirent *ep;
	char *newDirFile;
	
	if ((dp=opendir(dirFile))==NULL) {
		dirFile = replace_str(dirFile,get_setting(VIDEO_ROOT),"/mnt/raid/");
		add_file_to_playqueue(dirFile);
	} else {
		while((ep=readdir(dp))) {
			if (!strcmp(ep->d_name,".") || !strcmp(ep->d_name, "..")) { continue; } 

			#ifdef WIN32
				newDirFile = g_strdup_printf("%s\\%s", dirFile, ep->d_name);
			#else
				newDirFile = g_strdup_printf("%s/%s", dirFile, ep->d_name);
			#endif
			switch(file_type(newDirFile)){
				case FTDIR:
					add_folder_to_playqueue(newDirFile);
					break;
				case FTFILE:
					newDirFile = replace_str(newDirFile,get_setting(VIDEO_ROOT),"/mnt/raid/");
					add_file_to_playqueue(newDirFile);
					break;
				case FTDONOTPROC:
					break;
			}
		}
		closedir(dp);
	}
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
 
void about_box(GtkWidget *widget, gpointer gdata){
	#if VERBOSE >= 3
	g_printf("about Clicked\n");
	#endif
	GtkWidget *dialog, *label;

	/* Create the widgets */

	dialog = gtk_dialog_new_with_buttons ("Message", NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK, GTK_RESPONSE_NONE, NULL);
	label = gtk_label_new ("MSDaemon\nFancy Text here, maybe an icon...\nwho knows");

	/* Ensure that the dialog box is destroyed when the user responds. */

	g_signal_connect_swapped (dialog,
				"response",
				G_CALLBACK (gtk_widget_destroy),
				dialog);

	/* Add the label, and show everything we've added to the dialog. */
	#ifdef GTK3
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)),
			label);
	#else 
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox),
			label);
	#endif
	gtk_widget_show_all (dialog);
}

GtkStatusIcon* create_tray_icon() {
	/* Creates and returns the tray icon */
	GtkStatusIcon *tray_icon;
	#ifdef _WIN32
		#if VERBOSE >= 5
		g_printf ("Icon at: %s%s\n",PREFIX,"/share/rchip.png");
		#endif
		tray_icon = gtk_status_icon_new();
		gtk_status_icon_set_from_file (tray_icon,PREFIX "/share/rchip.png");
	#else
		tray_icon = gtk_status_icon_new_from_icon_name ("rchip-server");
	#endif
	g_signal_connect(G_OBJECT(tray_icon), "activate",G_CALLBACK(tray_click), NULL);
	g_signal_connect(G_OBJECT(tray_icon), "popup-menu", G_CALLBACK(tray_menu), NULL);
	gtk_status_icon_set_tooltip_text (tray_icon, "RCHIP");
	gtk_status_icon_set_visible(tray_icon, TRUE);
	return tray_icon;
}

GtkWidget* create_tray_menu(GtkStatusIcon* tray_icon) {
	/* Set's up the basic tray_menu, with four options */

	GtkWidget *tray_menu;
	GtkWidget *showsadd_item;
	GtkWidget *folderadd_item;
	GtkWidget *about_item;
	GtkWidget *quit_item;

	tray_menu = gtk_menu_new ();

	showsadd_item = gtk_menu_item_new_with_label ("Add Shows from files");
	folderadd_item = gtk_menu_item_new_with_label ("Add Shows from folders");
	about_item = gtk_menu_item_new_with_label ("About");
	quit_item = gtk_menu_item_new_with_label ("Quit");

	gtk_menu_shell_append (GTK_MENU_SHELL (tray_menu), showsadd_item);
	gtk_menu_shell_append (GTK_MENU_SHELL (tray_menu), folderadd_item);
	gtk_menu_shell_append (GTK_MENU_SHELL (tray_menu), about_item);	
	gtk_menu_shell_append (GTK_MENU_SHELL (tray_menu), quit_item);

	g_signal_connect_swapped(showsadd_item, "activate",G_CALLBACK(add_files),NULL);
	g_signal_connect_swapped(folderadd_item, "activate",G_CALLBACK(add_folders),NULL);
	g_signal_connect_swapped(about_item, "activate",G_CALLBACK(about_box),NULL);
	g_signal_connect_swapped(quit_item, "activate",gtk_main_quit,(gpointer) "file.quit");

	gtk_widget_show_all(tray_menu);
	return tray_menu;
}

