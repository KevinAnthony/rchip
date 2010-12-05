#include 	<glib.h>
#include	<glib-object.h>
#include	<dbus/dbus.h>
#include	<dbus/dbus-glib.h>
#include        <string.h>
struct playing_info{
        char *Artist;
        char *Album;
        char *Song;
        int Elapised_time;
        int Duration;
        int isPlaying;
};

gboolean dbus_is_connected(gboolean);
gboolean dbus_init();
gboolean send_command_to_player(char*);
gboolean send_command_to_player_with_argument(char*,GType, char*);
struct playing_info dbus_get_playing_info();
gchar* get_hash_str(GHashTable*, const char*);
unsigned int get_hash_uint(GHashTable*, const char*);
void print_playing_info(const struct playing_info);
