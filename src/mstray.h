#include <gtk/gtk.h>

	void start_tray();
	void tray_click(GtkStatusIcon *,gpointer);
	void tray_menu (GtkStatusIcon *, guint, guint, gpointer);
	void about_box(GtkWidget*, gpointer);
	GtkStatusIcon* create_tray_icon();
	GtkWidget* create_tray_menu(GtkStatusIcon*);

