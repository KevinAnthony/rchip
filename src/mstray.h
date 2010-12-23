#ifndef MSTRAY_H
#define MSTRAY_H

#include <gtk/gtk.h>

	void start_tray();
	void tray_click(GtkStatusIcon *,gpointer);
	void tray_menu (GtkStatusIcon *, guint, guint, gpointer);
	void add_shows(GtkWidget*, gpointer);
	void about_box(GtkWidget*, gpointer);
	//void file_ok_sel(GtkWidget*, GtkFileSelection);
	void file_ok_sel(GtkWidget*, GtkFileSelection*);

	GtkStatusIcon* create_tray_icon();
	GtkWidget* create_tray_menu(GtkStatusIcon*);
#endif
