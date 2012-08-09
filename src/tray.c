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

#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <string.h>
#include <glob.h>

#include "tray.h"
#include "showlist.h"
#include "utils.h"
#include "settings.h"
#include "status.h"
#include "xml.h"

extern GAsyncQueue  *file_async_queue;
extern GAsyncQueue  *gui_async_queue;
extern GAsyncQueue  *network_async_queue;
extern GThread      *network_thread;
extern GThread      *file_thread;
extern GThread      *gui_thread;

void start_tray(){
#ifdef GTK3
    /* set up a new GtkApplication and register callback*/
    GtkApplication *app;
    app = gtk_application_new("org.noside.rchip", 0);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    g_application_register(G_APPLICATION(app),NULL,NULL);
    /*
     * we check to see if this application is primary(or not remote) and
     * run it if it's primary otherwise, we let it die
     * because we would have nested gtk_main loops, which causes quit to
     * not function properly
     */

    if (!(g_application_get_is_remote(G_APPLICATION(app))))
        g_application_run(G_APPLICATION (app),0,NULL);

    g_object_unref(app);
#else
    gtk_main ();
#endif
}

#if GTK_MAJOR_VERSION >= 3
void activate (GtkApplication *app)
{
    print("Application Activated","org.noside.rchip",INFO);
    gtk_main();
}
#endif

void tray_click(GtkStatusIcon *status_icon,gpointer user_data)
{
    /* on primary click(default:left) we either show or hide the status window */
    show_hide_window();
}

void tray_menu(GtkStatusIcon *status_icon, guint button, guint activate_time, gpointer user_data)
{
    /* on secondary clicks(default:right) we show the menu */
    GtkWidget *tray_menu = create_tray_menu(status_icon);
    gtk_menu_popup (GTK_MENU (tray_menu), NULL, NULL,gtk_status_icon_position_menu,status_icon,button,activate_time);
}

void credentials(GtkWidget *widget, gpointer gdata){
    static GtkWidget *dialog = NULL;
    dialog = gtk_dialog_new();
    gtk_window_set_modal( GTK_WINDOW( dialog ), TRUE );
    //gtk_window_set_transient_for( GTK_WINDOW( dialog ),GTK_WINDOW( window ) );

    gtk_window_set_title( GTK_WINDOW( dialog ), "Conformation" );

    gtk_dialog_add_button( GTK_DIALOG( dialog ), GTK_STOCK_YES, 1 );
    gtk_dialog_add_button( GTK_DIALOG( dialog ), GTK_STOCK_NO,  2 );
    GtkWidget *box = gtk_dialog_get_content_area( GTK_DIALOG( dialog ) );

    GtkEntry *username = gtk_entry_new();
    GtkEntry *password = gtk_entry_new();
    gtk_entry_set_text (username, get_setting_str(REST_USERNAME));
    gtk_entry_set_text (password, get_setting_str(REST_PASSWORD));
    gtk_entry_set_visibility(GTK_ENTRY(password),FALSE);


    gtk_box_pack_start( GTK_BOX( box ), gtk_label_new( "Username" ), TRUE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( box ), GTK_WIDGET(username), TRUE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( box ), gtk_label_new( "Password" ), TRUE, TRUE, 0 );
    gtk_box_pack_start( GTK_BOX( box ), GTK_WIDGET(password), TRUE, TRUE, 0 );

    gtk_widget_show_all( dialog );
    if (gtk_dialog_run( GTK_DIALOG (dialog) ) == 1){
        set_setting_str(REST_USERNAME,gtk_entry_get_text(username));
        set_setting_str(REST_PASSWORD,gtk_entry_get_text(password));
    }
    gtk_widget_hide( dialog );
}
void add_files(GtkWidget *widget, gpointer gdata){
    /*Add Show files to remote watch list*/
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new ("Open File",
            NULL,
            GTK_FILE_CHOOSER_ACTION_OPEN,
            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
            GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
            NULL);
    gtk_window_set_icon_name(dialog,"rchip-server");
    /* default to the VIDEO ROOT path */
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER (dialog),get_setting_str(VIDEO_ROOT));
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
            print("Adding Filename",filename,INFO);
            /* because the user may not use the same machine to watch as they did to add, we replace the local path with a server absoulte path*/
            filename = replace_str(filename,get_setting_str(VIDEO_ROOT),"/mnt/raid/");
            queue_function_data* func = g_malloc(sizeof(queue_function_data));
            func->func  = *add_file_to_playqueue;
            func->data = g_strdup(filename);
            func->priority = TP_NORMAL;
            g_async_queue_push_sorted(file_async_queue,(gpointer)func,(GCompareDataFunc)sort_async_queue,NULL);
            node=node->next;
        }
        g_slist_free(filelist);
    }
    gtk_widget_destroy (dialog);

}

void add_folders(GtkWidget *widget, gpointer gdata){
    /* this works the same as the add_files above, but recursivly adds folders instead of single or multiple files in 1 folder */
    GtkWidget *dialog;
    dialog = gtk_file_chooser_dialog_new ("Open File",
            NULL,
            GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
            GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
            NULL);
    gtk_window_set_icon_name(dialog,"rchip-server");
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER (dialog),get_setting_str(VIDEO_ROOT));
    gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (dialog),TRUE);
    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT){
        char *filename;
        GSList* filelist = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER (dialog));
        GSList* node = filelist;
        while (node != NULL){
            filename = (char*)node->data;
            print("Adding Filename",filename,DEBUG);
            queue_function_data* func = g_malloc(sizeof(queue_function_data));
            func->func  = *add_folder_to_playqueue;
            func->data = (gpointer*)g_strdup(filename);
            func->priority = TP_NORMAL;
            g_async_queue_push_sorted(file_async_queue,(gpointer)func,(GCompareDataFunc)sort_async_queue,NULL);
            node=node->next;
        }
        g_slist_free(filelist);
        //g_free (filename);
    }
    gtk_widget_destroy (dialog);

}

void on_quit(GtkWidget *widget, gpointer gdata){
    gint* exit = THREAD_EXIT;
    g_async_queue_push(network_async_queue,(gpointer)exit);
    g_thread_join ( network_thread );
    g_async_queue_push(file_async_queue,(gpointer)exit);
    g_thread_join ( file_thread );
    g_async_queue_push(gui_async_queue,(gpointer)exit);
    g_thread_join ( gui_thread );

    gtk_main_quit();
}
void about_box(GtkWidget *widget, gpointer gdata){
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

void change_music(GtkWidget *widget, gpointer gdata){
    if(!set_setting_str(MUSIC_XML,(char*)gdata)){
        g_warning("Couldn't set MusicXML\n");
    }
    g_free(gdata);
}
void change_video(GtkWidget *widget, gpointer gdata){
    if(!set_setting_str(VIDEO_XML,(char*)gdata)){
        g_warning("Couldn't set VideoXML\n");
    }
    g_free(gdata);
}

GtkStatusIcon* create_tray_icon() {
    /* Creates and returns the tray icon */
    GtkStatusIcon *tray_icon;
    tray_icon = gtk_status_icon_new_from_icon_name ("rchip-server");
    //tray_icon = gtk_status_icon_new_from_file("/usr/local/share/icons/hicolor/48x48/apps/rchip-server.png");
    g_signal_connect(G_OBJECT(tray_icon), "activate",G_CALLBACK(tray_click), NULL);
    g_signal_connect(G_OBJECT(tray_icon), "popup-menu", G_CALLBACK(tray_menu), NULL);
    gtk_status_icon_set_tooltip_text (tray_icon, "RCHIP");
    gtk_status_icon_set_visible(tray_icon, TRUE);
    return tray_icon;
}

GtkWidget* create_tray_menu(GtkStatusIcon* tray_icon) {
    /* Set's up the basic tray_menu, with four options */

    GtkWidget *tray_menu;

    GtkWidget *credentials_item;
    GtkWidget *showsadd_item;
    GtkWidget *folderadd_item;
    GtkWidget *about_item;
    GtkWidget *quit_item;

    GtkWidget *setting_menu;
    GtkWidget *music_menu;
    GtkWidget *video_menu;

    GtkWidget *setting_item;
    GtkWidget *music_item;
    GtkWidget *video_item;


    tray_menu = gtk_menu_new ();

    setting_menu = gtk_menu_new ();
    music_menu = gtk_menu_new ();
    video_menu = gtk_menu_new ();

    setting_item = gtk_menu_item_new_with_label ("Settings");
    music_item = gtk_menu_item_new_with_label ("Music Program");
    video_item = gtk_menu_item_new_with_label ("Video Program");
    credentials_item = gtk_menu_item_new_with_label ("Login Credentials");

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(setting_item),setting_menu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(music_item),music_menu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(video_item),video_menu);

    set_xml_menu(music_menu,video_menu);

    showsadd_item = gtk_menu_item_new_with_label ("Add Shows from files");
    folderadd_item = gtk_menu_item_new_with_label ("Add Shows from folders");
    about_item = gtk_menu_item_new_with_label ("About");
    quit_item = gtk_menu_item_new_with_label ("Quit");

    gtk_menu_shell_append(GTK_MENU_SHELL (setting_menu),music_item);
    gtk_menu_shell_append(GTK_MENU_SHELL (setting_menu),video_item);

    gtk_menu_shell_append (GTK_MENU_SHELL (tray_menu), credentials_item);
    gtk_menu_shell_append (GTK_MENU_SHELL (tray_menu), gtk_separator_menu_item_new());
    gtk_menu_shell_append (GTK_MENU_SHELL (tray_menu), showsadd_item);
    gtk_menu_shell_append (GTK_MENU_SHELL (tray_menu), folderadd_item);
    gtk_menu_shell_append (GTK_MENU_SHELL (tray_menu), gtk_separator_menu_item_new());
    gtk_menu_shell_append (GTK_MENU_SHELL (tray_menu), setting_item);
    gtk_menu_shell_append (GTK_MENU_SHELL (tray_menu), gtk_separator_menu_item_new());
    gtk_menu_shell_append (GTK_MENU_SHELL (tray_menu), about_item);
    gtk_menu_shell_append (GTK_MENU_SHELL (tray_menu), quit_item);

    g_signal_connect_swapped(credentials_item, "activate",G_CALLBACK(credentials),NULL);
    g_signal_connect_swapped(showsadd_item, "activate",G_CALLBACK(add_files),NULL);
    g_signal_connect_swapped(folderadd_item, "activate",G_CALLBACK(add_folders),NULL);
    g_signal_connect_swapped(about_item, "activate",G_CALLBACK(about_box),NULL);
    g_signal_connect_swapped(quit_item, "activate",G_CALLBACK(on_quit),(gpointer) "file.quit");

    gtk_widget_show_all(tray_menu);
    return tray_menu;
}

void set_xml_menu(GtkWidget *music_menu, GtkWidget *video_menu){
    set_xml_menu_with_path(music_menu,video_menu,PREFIX"/noside/xml/");
    set_xml_menu_with_path(music_menu,video_menu,"~/.noside/xml/");
}
void set_xml_menu_with_path(GtkWidget *music_menu, GtkWidget *video_menu,char* path){
    glob_t data;
    switch( glob(g_strconcat(path,"*.xml",NULL), 0, NULL, &data ) ){
        case 0:
            break;
        case GLOB_NOSPACE:
            print("Out of memory","You Should NEVER see this, unless it's doomsday, then it's OK", ERROR );
            break;
        case GLOB_ABORTED:
            print( "Reading error",NULL,ERROR );
            break;
        default:
            break;
    }
    for (int i =0;i<data.gl_pathc; i++){
        char* posOfLastSlash=data.gl_pathv[i];
        char* ptr = data.gl_pathv[i];
        char* sptr = data.gl_pathv[i];
        char* progName;
        char* progType;
        char* nameP;
        char* typeP;
        progName = g_malloc(strlen(ptr));
        progType = g_malloc(strlen(ptr));
        nameP = progName;
        typeP = progType;
        for (; *ptr != '\0';ptr++) {
            if (*ptr == '/') {
                posOfLastSlash = ptr;
            }
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
                    g_warning("File Name: %s\n",data.gl_pathv[i]);
#endif
                    return;
                }
                if (i == 0){ *nameP++=*ptr; }
                else if (i == 1){ *typeP++=*ptr; }
                else if (i == 2){ break; }
            }
            ptr++;
        }
        *nameP='\0';
        *typeP='\0';
        *progName= g_ascii_toupper(*progName);
        if (!(g_strcmp0(progType,"music"))){
            GtkWidget *item;
            item = gtk_menu_item_new_with_label(progName);
            gtk_menu_shell_append(GTK_MENU_SHELL (music_menu), item);
            char* filePath= g_strconcat(path,sptr,NULL);
            g_signal_connect(item,"activate",G_CALLBACK(change_music),(gpointer)filePath);
        }else if (!(g_strcmp0(progType,"video"))){
            GtkWidget *item2;
            item2 = gtk_menu_item_new_with_label(progName);
            gtk_menu_shell_append(GTK_MENU_SHELL (video_menu), item2);
            char* filePath= g_strconcat(path,sptr,NULL);
            g_signal_connect(item2,"activate",G_CALLBACK(change_video),(gpointer)filePath);
        }
        g_free(progName);
        g_free(progType);
    }
    globfree( &data );
}
