#include	"sql.h"
#include	<glib.h>
#include        <dbus/dbus.h>
#include        <dbus/dbus-glib.h>

gboolean setNotification(char*,char*,char*);
void getTorrentInfo(DBusGProxy*,char*);
