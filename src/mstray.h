#ifndef MSTRAY_H
#define MSTRAY_H

#include <gtk/gtk.h>
#include <dirent.h>
	enum {
		FTDIR,
  		FTFILE,
  		FTDONOTPROC
	};
	void start_tray();
	void tray_click(GtkStatusIcon *,gpointer);
	void tray_menu (GtkStatusIcon *, guint, guint, gpointer);
	void add_files(GtkWidget*, gpointer);
	void add_folders(GtkWidget*, gpointer);	
	void about_box(GtkWidget*, gpointer);
	void add_folder_to_playqueue(char*);
	int fileType(char*);
	GtkStatusIcon* create_tray_icon();
	GtkWidget* create_tray_menu(GtkStatusIcon*);
#endif
