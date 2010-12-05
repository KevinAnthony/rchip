#include <glib.h>

struct hostname_list {
        char* hostname;
        struct hostname_list *next;
};

typedef struct hostname_list hn_list;

gboolean add_to_hosts(char*);
gboolean delete_from_hosts(char*);
char* get_next_host();
void print_list();
void get_next_cmd();
