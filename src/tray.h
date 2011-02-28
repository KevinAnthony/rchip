#ifndef TRAY_H
#define TRAY_H
#if GTK_MAJOR_VERSION >= 3
#define GTK3
#else
#define GTK2
#endif
#include <gtk/gtk.h>
#include <dirent.h>
	enum {
		FTDIR,
  		FTFILE,
  		FTDONOTPROC
	};
	void start_tray();
#ifdef GTK3
	void activate (GtkApplication*);
#endif
	void tray_click(GtkStatusIcon*,gpointer);
	void tray_menu (GtkStatusIcon*, guint, guint, gpointer);
	void add_files(GtkWidget*, gpointer);
	void add_folders(GtkWidget*, gpointer);	
	void about_box(GtkWidget*, gpointer);
	void add_folder_to_playqueue(char*);
	int file_type(char*);
	GtkStatusIcon* create_tray_icon();
	GtkWidget* create_tray_menu(GtkStatusIcon*);
#endif
