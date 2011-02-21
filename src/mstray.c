#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
	#include <windows.h>
#endif
#include "mstray.h"
#include "showlist.h"
#include "utils.h"
#include "settings.h"

void start_tray(){
	printf("GTK:%i\n",GTK_MAJOR_VERSION);
	#if GTK_MAJOR_VERSION >= 3
		GtkApplication *app;
		gint status;
		app = gtk_application_new("org.noside.msdaemon", 0);
		status = gtk_application_run(app);
		g_object_unref(app);
	#else
		gtk_main ();
	#endif
}

void tray_click(GtkStatusIcon *status_icon,gpointer user_data)
{
	#ifdef _DEBUG
		printf("clicked on Icon\n");
	#endif
}

void tray_menu(GtkStatusIcon *status_icon, guint button, guint activate_time, gpointer user_data)
{
	#ifdef _DEBUG
		printf("clicked on menu\n");
	#endif
	GtkWidget *tray_menu = create_tray_menu(status_icon);
	gtk_menu_popup (GTK_MENU (tray_menu), NULL, NULL,gtk_status_icon_position_menu,status_icon,button,activate_time);
}

void add_files(GtkWidget *widget, gpointer gdata){
 	#ifdef _DEBUG
                printf("addShows Clicked\n");
	#endif
	GtkWidget *dialog;
	dialog = gtk_file_chooser_dialog_new ("Open File",
				      NULL,
				      GTK_FILE_CHOOSER_ACTION_OPEN,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				      NULL);
	gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (dialog),TRUE);
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT){
		char *filename;
		GSList* filelist =  gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER (dialog));
    		GSList* node = filelist;
		while (node != NULL){
			filename = (char*)node->data;
			#ifdef _WIN32
			char* p = filename;
			while (*p++ != '\0'){ if (*p == '\\') { *p = '/';} }
			#endif
			printf("%s\n",filename);
			filename = replace_str(filename,getsetting("pathToRoot"),"/mnt/raid/");
			add_file_to_playqueue(filename);
			node=node->next;
		}
		g_slist_free(filelist);
    		//g_free (filename);
  	}
	gtk_widget_destroy (dialog);

}

void add_folders(GtkWidget *widget, gpointer gdata){
        #ifdef _DEBUG
                printf("addFolders Clicked\n");
        #endif
        GtkWidget *dialog;
        dialog = gtk_file_chooser_dialog_new ("Open File",
                                      NULL,
                                      GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
                                      NULL);
        gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (dialog),TRUE);
        if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT){
                char *filename;
                GSList* filelist =  gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER (dialog));
                GSList* node = filelist;
                while (node != NULL){
                        filename = (char*)node->data;
                        #ifdef _WIN32
                        char* p = filename;
                        while (*p++ != '\0'){ if (*p == '\\') { *p = '/';} }
                        #endif
                        printf("%s\n",filename);
                        //filename = replace_str(filename,getsetting("pathToRoot"),"/mnt/raid/");
                        add_folder_to_playqueue(filename);
                        node=node->next;
                } 
                g_slist_free(filelist);
                //g_free (filename);  
        }                             
        gtk_widget_destroy (dialog);  
                                      
}

void add_folder_to_playqueue(char *dirFile){
  	DIR    *dp;
  	struct dirent *ep;
  	char *newDirFile;
	
  	if ((dp=opendir(dirFile))==NULL) {
		dirFile = replace_str(dirFile,getsetting("pathToRoot"),"/mnt/raid/");
		add_file_to_playqueue(dirFile);
  	} else  {
		while((ep=readdir(dp))) {
      			if (!strcmp(ep->d_name,".") ||  !strcmp(ep->d_name, "..")) { continue; } 

			#ifdef WIN32
      				newDirFile = g_strdup_printf("%s\\%s", dirFile, ep->d_name);  
			#else
      				newDirFile = g_strdup_printf("%s/%s", dirFile, ep->d_name);  
			#endif
      			switch(fileType(newDirFile)){
        			case FTDIR:
          				add_folder_to_playqueue(newDirFile);
        				break;
        			case FTFILE:
					newDirFile = replace_str(newDirFile,getsetting("pathToRoot"),"/mnt/raid/");
        				add_file_to_playqueue(newDirFile);
					break;
        			case FTDONOTPROC:
        				break;
      			}
    		}
    		closedir(dp);
  	}
}

int fileType(char *name){
	DIR    *dp;
	int   ret = FTFILE;
	if (!((dp=opendir(name))==NULL)){
		ret = FTDIR;
    		closedir(dp);
  	}
  	return ret;
} 
 
void about_box(GtkWidget *widget, gpointer gdata){
	#ifdef _DEBUG
		printf("about Clicked\n");
	#endif
	GtkWidget *dialog, *label;
     
        /* Create the widgets */
     
        dialog = gtk_dialog_new_with_buttons ("Message", NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_OK,  GTK_RESPONSE_NONE, NULL);
       	label = gtk_label_new ("MSDaemon\nFancy Text here, maybe an icon...\nwho knows");
     
        /* Ensure that the dialog box is destroyed when the user responds. */
     
        g_signal_connect_swapped (dialog,
                                  "response",
                                  G_CALLBACK (gtk_widget_destroy),
                                  dialog);
     
        /* Add the label, and show everything we've added to the dialog. */
     
        gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox),
                           label);
        gtk_widget_show_all (dialog);
}

GtkStatusIcon* create_tray_icon() {
        GtkStatusIcon *tray_icon;
        tray_icon = gtk_status_icon_new();
        g_signal_connect(G_OBJECT(tray_icon), "activate",G_CALLBACK(tray_click), NULL);
        g_signal_connect(G_OBJECT(tray_icon), "popup-menu", G_CALLBACK(tray_menu), NULL);
	#ifdef PREFIX
		#ifdef _WIN32
        		gtk_status_icon_set_from_file (tray_icon,PREFIX "/share/msdaemon.png");
		#else
			gtk_status_icon_set_from_file (tray_icon,PREFIX "/share/madsci/msdaemon.png");
		#endif
	#else
		gtk_status_icon_set_from_icon_name(tray_icon,GTK_STOCK_MEDIA_STOP);
	#endif
	gtk_status_icon_set_tooltip(tray_icon, "MS Deamon");
        gtk_status_icon_set_visible(tray_icon, TRUE);
        return tray_icon;
}

GtkWidget* create_tray_menu(GtkStatusIcon* tray_icon) {
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
	gtk_menu_append (GTK_MENU_SHELL (tray_menu), showsadd_item);
	gtk_menu_append (GTK_MENU_SHELL (tray_menu), folderadd_item);
	gtk_menu_append (GTK_MENU_SHELL (tray_menu), about_item);	
	gtk_menu_append (GTK_MENU_SHELL (tray_menu), quit_item);
	gtk_signal_connect_object (GTK_OBJECT (showsadd_item), "activate",G_CALLBACK(add_files),NULL);
	gtk_signal_connect_object (GTK_OBJECT (folderadd_item), "activate",G_CALLBACK(add_folders),NULL);
	gtk_signal_connect_object (GTK_OBJECT (about_item), "activate",G_CALLBACK(about_box),NULL);
	gtk_signal_connect_object (GTK_OBJECT (quit_item), "activate",(GtkSignalFunc) gtk_main_quit,(gpointer) "file.quit");
	gtk_widget_show_all(tray_menu);
	return tray_menu;
}

