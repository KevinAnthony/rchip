#include	"sql.h"
#include	<glib.h>
#ifndef _WIN32
	#include        <dbus/dbus.h>
	#include        <dbus/dbus-glib.h>
#endif
gboolean setNotification(char*,char*,char*);
#ifndef _WIN32
	void getTorrentInfo(DBusGProxy*,char*);
#endif