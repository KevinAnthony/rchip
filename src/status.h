#ifndef STATUS_H
#define STATUS_H
#include <gtk/gtk.h>

	void init_status_window(gboolean);
	void init_status_labels();
	void init_info_labels();
	void init_buttons();
	void window_destroyed(GtkWidget*,gpointer);
	void close_window(GtkWidget*,gpointer);
	void show_hide_window();
#endif
