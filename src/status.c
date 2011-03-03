#include <config.h>

#include <gtk/gtk.h>
#include "status.h"

GtkBuilder *builder;
GtkWidget *window;

void init_status_window (gboolean showWindow){
	builder = gtk_builder_new ();
	gtk_builder_add_from_file (builder, PREFIX "/noside/ui/rchip_status_window.glade", NULL);
	window = GTK_WIDGET (gtk_builder_get_object (builder, "statusWindow"));
	//g_object_unref (G_OBJECT (builder));
	init_info_labels();
	init_status_labels();
	init_buttons();
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(window_destroyed), NULL);
	if (showWindow){
	        gtk_widget_show (window);
	}
}

void init_status_labels(){
	GtkLabel *label;
	label = GTK_LABEL (gtk_builder_get_object (builder, "version"));
	gtk_label_set_label(label,PACKAGE_VERSION);
	label = GTK_LABEL (gtk_builder_get_object (builder, "programname"));
        gtk_label_set_label(label,PACKAGE_NAME);
	label = GTK_LABEL (gtk_builder_get_object (builder, "programersEmail"));
        gtk_label_set_label(label,PACKAGE_BUGREPORT);
}

void init_info_labels(){
	GtkLabel *label;
	#ifdef RHYTHMBOX
		label = GTK_LABEL (gtk_builder_get_object (builder, "music_player"));
	        gtk_label_set_label(label,"Using Rhythmbox");
	#endif
	#ifdef BANSHEE
                label = GTK_LABEL (gtk_builder_get_object (builder, "music_player"));
                gtk_label_set_label(label,"Using Banshee");
        #endif
	#ifdef _NOSQL
		label = GTK_LABEL (gtk_builder_get_object (builder, "sql_status"));
                gtk_label_set_label(label,"SQL Disabled");
        #endif
	return;
}

void init_buttons(){
	GtkWidget *button;
	button = GTK_WIDGET(gtk_builder_get_object (builder, "close"));
	g_signal_connect(G_OBJECT(button), "clicked",G_CALLBACK(close_window),NULL);
	button = GTK_WIDGET (gtk_builder_get_object (builder, "quit"));
        g_signal_connect(G_OBJECT(button), "clicked", gtk_main_quit,(gpointer) "file.quit");
	return;
}

void window_destroyed(GtkWidget *widget, gpointer gdata){
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

