#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mstray.h"

void start_tray(){
	gtk_main();
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
	//gtk_menu_popup (GTK_MENU (tray_menu), NULL, NULL,NULL,NULL,button, activate_time);
	gtk_menu_popup (GTK_MENU (tray_menu), NULL, NULL,gtk_status_icon_position_menu,status_icon,button,activate_time);
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
        	gtk_status_icon_set_from_file (tray_icon,PREFIX "/share/madsci/msdaemon.png");
	#else
		gtk_status_icon_set_from_icon_name(tray_icon,GTK_STOCK_MEDIA_STOP);
	#endif
	gtk_status_icon_set_tooltip(tray_icon, "MS Deamon");
        gtk_status_icon_set_visible(tray_icon, TRUE);
        return tray_icon;
}

GtkWidget* create_tray_menu(GtkStatusIcon* tray_icon) {
	GtkWidget *tray_menu;
	GtkWidget *about_item;
	GtkWidget *quit_item;
	tray_menu = gtk_menu_new ();
	about_item = gtk_menu_item_new_with_label ("About");
    	quit_item = gtk_menu_item_new_with_label ("Quit");
	gtk_menu_append (GTK_MENU_SHELL (tray_menu), about_item);	
	gtk_menu_append (GTK_MENU_SHELL (tray_menu), quit_item);
	gtk_signal_connect_object (GTK_OBJECT (about_item), "activate",G_CALLBACK(about_box),NULL);
	gtk_signal_connect_object (GTK_OBJECT (quit_item), "activate",(GtkSignalFunc) gtk_main_quit,(gpointer) "file.quit");
	gtk_widget_show_all(tray_menu);
	return tray_menu;
}

