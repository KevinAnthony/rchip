#ifndef SQL_H
#define SQL_H

#ifdef _WIN32
	#include        <winsock.h>
	#include        <mysql.h>
#else
	#include	<mysql/mysql.h>
#endif
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
void update_daemon_sql();
#endif
