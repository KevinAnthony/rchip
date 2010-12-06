#include        <mysql/mysql.h>
#include	<glib.h>

gboolean sql_init();
int sql_is_connected(int);
void sql_exec_quary(char*);
void get_next_cmd_from_sql(char*,int*,char**,char**,char**);
void delete_from_cmdQueue(int);
char* get_registered_devices_message();
void get_active_devices(void*, size_t, size_t);
size_t get_size();
size_t get_nelem();
