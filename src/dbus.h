#ifndef _WIN32
#ifndef DBUS_H
#define DBUS_H

#include	<gio/gio.h>
#include        <string.h>
#include 	"utils.h"

gboolean dbus_is_connected(gboolean);
gboolean dbus_init();
gboolean send_command_to_music_player(char*);	
gboolean send_command_to_music_player_with_argument(char*,char*, char*);
struct playing_info_music dbus_get_playing_info_music();
gchar* get_hash_str(GHashTable*, const char*);	
unsigned int get_hash_uint(GHashTable*, const char*);
void print_playing_info_music(const struct playing_info_music);

#endif
#endif
