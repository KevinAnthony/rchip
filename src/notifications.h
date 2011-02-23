#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H

#include	"sql.h"
#include	<glib.h>

#ifndef _WIN32
#include        <dbus/dbus.h>
#include        <dbus/dbus-glib.h>
#endif

gboolean set_notification(char*,char*,char*);

#ifndef _WIN32
void get_torrent_info(DBusGProxy*,char*);
#endif

#endif
