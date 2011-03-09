/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
*
*    rchip, Remote Controlled Home Integration Program
*    Copyright (C) 2011 <Kevin@NoSideRacing.com>
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*/

#include <config.h>

#ifdef _SQL

#ifdef _WIN32
	#include 	<winsock.h>
	#include	<mysql.h>
	#include 	<windows.h>
#else
	#include	<mysql/mysql.h>
	#include 	<sys/utsname.h>
#endif
#include 	<glib.h>
#include	<glib/gprintf.h>
#include	<string.h>
#include	<stdlib.h>
#include 	"sql.h"
#include	"settings.h"

#define		HOSTNAME		"192.168.1.3"
#define 	USERNAME		"nTesla"
#define		PASSWORD		"deathray"
#define		DATABASE		"madSci"
#define		HOSTNAME_MAX_SIZE	256
/*  
 *  Here we would if tryOnDisconneced is true, and we are Disconneced
 *  We would try and connect
 */
MYSQL *mysql = NULL;


gboolean sql_is_connected (int tryOnDisconneced) {
	if (tryOnDisconneced) {
		return 0;
	} else {
		return 0;
	}
}

gboolean sql_init ()
{
	if (mysql!=NULL){
		mysql_close(mysql);
		mysql=NULL;
	}
	mysql =	mysql_init(mysql);
	if (mysql == NULL) {
		#if VERBOSE >= 1
		g_error("Error init %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
		#endif
		return FALSE;
	}
	return TRUE;
}

void sql_exec_quary (char* query) {
	sql_init();
	#if VERBOSE >= 4 
	g_printf("Query:%s\n",query);	
	#endif
	if (!sql_init()){ return; }
	if (mysql_real_connect(mysql,HOSTNAME,USERNAME,PASSWORD,DATABASE,0,NULL,0) == NULL) {
		#if VERBOSE >= 1	
		g_error("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
		#endif
		mysql_close(mysql);
		mysql=NULL;
		#if VERBOSE >= 1
		g_error("Returning because real_connect failed in sql_exec_quary");
		#endif
		return;
	}
	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){
		#if VERBOSE >= 1			
		g_error("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
		#endif
		mysql_close(mysql);
		mysql=NULL;
		#if VERBOSE >= 1
		g_error("Returning because real_query failed in sql_exec_quary");
		#endif
		return;
	}
	mysql_close(mysql);
	mysql=NULL;
}

void get_next_cmd_from_sql(char *hostname,int* cmdID,char** cmd,char** cmdTxt, char** source) {
	char *query;
	query = (char*) g_malloc (sizeof("select * from cmdQueue where dest_hostname = '")+sizeof(hostname)+sizeof("';")+5);
	g_sprintf(query,"select * from cmdQueue where dest_hostname = '%s';",hostname);
	MYSQL_RES *res;
	MYSQL_ROW row;
	#if VERBOSE >= 4 
	g_printf("Next Cmd Query:%s\n",query);
	#endif	
	if (!sql_init()){ return; }
	if (mysql_real_connect(mysql,HOSTNAME,USERNAME,PASSWORD,DATABASE,0,NULL,0) == NULL) {
		#if VERBOSE >= 1	
		g_error("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
		#endif
		mysql_close(mysql);
		mysql=NULL;
		#if VERBOSE >= 1
		g_error("Returning because real_connect failed in get_next_cmd_from_sql");
		#endif
		return;
	} 
	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){
		#if VERBOSE >= 1
		g_error("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
		#endif
		mysql_close(mysql);
		mysql=NULL;
		#if VERBOSE >= 1
		g_error("Returning because real_query failed in get_next_cmd_from_sql");
		#endif
		return; 
	}

	res = mysql_use_result(mysql);
	if (res == NULL) { 
		mysql_close(mysql);
		mysql=NULL;
		#if VERBOSE >= 1
		g_error("Returning because res == NULL in get_next_cmd_from_sql");
		#endif
		return;
	}
	if ((row = mysql_fetch_row(res))) {
	
		*cmd = (char*)g_malloc(1024);
		*cmdTxt = (char*)g_malloc(1024);
		*source = (char*)g_malloc(1024);
		*cmdID = atoi(row[0] ? row[0] : NULL);
		g_sprintf(*cmd,"%s",row[2] ? row[2] : NULL);
		g_sprintf(*cmdTxt,"%s",row[3] ? row[3] : NULL);
		g_sprintf(*source,"%s",row[4] ? row[4] : NULL);
	} else {
		*cmd = " ";
		*cmdTxt = " ";
		*source = " ";
		*cmdID = 0;
	}
	mysql_free_result(res);
	mysql_close(mysql);
	mysql=NULL;
	g_free(query);
}

void delete_from_cmdQueue(int cmdID) {
	char *delquery;
	delquery = (char*) g_malloc (sizeof("delete from cmdQueue where id =;")+sizeof(cmdID));
	g_sprintf(delquery,"delete from cmdQueue where id =%i;",cmdID);
	#if VERBOSE >= 4
	g_printf("Next Cmd Query:%s\n",delquery);
	#endif
	if (!sql_init()){ return; }
	if (mysql_real_connect(mysql,HOSTNAME,USERNAME,PASSWORD,DATABASE,0,NULL,0) == NULL) {
		#if VERBOSE >= 1
		g_error("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
		#endif
		mysql_close(mysql);
		mysql=NULL;
		#if VERBOSE >= 1
		g_error("Returning because real_connect failed in delete_from_cmdQueue");
		#endif
		return;
	}
	if(mysql_real_query(mysql,delquery,(unsigned int)strlen(delquery))){
		#if VERBOSE >= 1
		g_error("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
		g_error("Returning because real_query failed in delete_from_cmdQueue");
		#endif
		mysql_close(mysql);
		mysql=NULL;
		return;
	}
	mysql_close(mysql);
	mysql=NULL;
	g_free(delquery);
}

char* get_registered_devices_message(){
	char* query = "select source_hostname from messageRegister where active = \"TRUE\"";
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* returnval = (char*) g_malloc(1024);
	if (!sql_init()){ return NULL; }
	if (mysql_real_connect(mysql,HOSTNAME,USERNAME,PASSWORD,DATABASE,0,NULL,0) == NULL) {
		#if VERBOSE >= 1
		g_error("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
		#endif
		mysql_close(mysql);
		mysql=NULL;
		#if VERBOSE >= 1
		g_error("Returning because real_connect failed in get_registered_devices_message");
		#endif
		return NULL;
	}
	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){
		#if VERBOSE >= 1	
		g_error("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
		#endif
		mysql_close(mysql);
		mysql=NULL;
		#if VERBOSE >= 1
		g_error("Returning because real_query failed in get_registered_devices_message");
		#endif
		return NULL;
	}

	res = mysql_use_result(mysql);
	if (res == NULL) {
		mysql_close(mysql);
		mysql=NULL;
		#if VERBOSE >= 1
		g_error("Returning because res == NULL in get_registered_devices_message");
		#endif
		return NULL;
	}
	row=mysql_fetch_row(res);
	g_sprintf(returnval,"%s",row[0]);
	while ((row = mysql_fetch_row(res))) {
		returnval=strcat(returnval,row[0]);
	}
	mysql_free_result(res);
	mysql_close(mysql);
	mysql=NULL;
	return returnval;
}

size_t get_nelem(){
	char* query = "select hostname from remoteActiveDevices where active = \"TRUE\"";
	MYSQL_RES *res;
	if (!sql_init()){ return -1; }
	if (mysql_real_connect(mysql,HOSTNAME,USERNAME,PASSWORD,DATABASE,0,NULL,0) == NULL) {
		#if VERBOSE >= 1  
		g_error("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
		g_error("Returning because real_connect failed in get_nelem");
		#endif
		mysql_close(mysql);
		mysql=NULL;
		return -1;
	}

	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){
		#if VERBOSE >= 1	
		g_error("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
		#endif
		mysql_close(mysql);
		mysql=NULL;
		#if VERBOSE >= 1
		g_error("Returning because real_query failed in get_nelem");
		#endif
		return -1;
	}
	res = mysql_store_result(mysql);
	size_t retval = (int)mysql_num_rows(res);
	mysql_free_result(res);
	mysql_close(mysql);
	mysql=NULL;
	return retval;
}

size_t get_size(){
	return HOSTNAME_MAX_SIZE;
}

void get_active_devices(void* base, size_t size, size_t nelem)
{	
	char* query = "select hostname from remoteActiveDevices where active = \"TRUE\";";
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* hostname = (char*) g_malloc(HOSTNAME_MAX_SIZE);
	if (!sql_init()){ return; }
	if (mysql_real_connect(mysql,HOSTNAME,USERNAME,PASSWORD,DATABASE,0,NULL,0) == NULL) {
		#if VERBOSE >= 1	
		g_error("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
		#endif
		mysql_close(mysql);
		mysql=NULL;
		#if VERBOSE >= 1
		g_error("Returning because real_connect failed in get_active_device");
		#endif
		return;
	}
	
	
	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){
		#if VERBOSE >= 1	
		g_error("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
		#endif
		mysql_close(mysql);
		mysql=NULL;
		#if VERBOSE >= 1
		g_error("Returning because real_query failed in get_active_device");
		#endif
		return;
	}
	res = mysql_use_result(mysql);

	if (res == NULL) {
		mysql_close(mysql);
		mysql=NULL;
		#if VERBOSE >= 1
		g_error("Returning because res == NULL in get_active_device");
		#endif
		return;
	}
	size_t len = 0;	
	
	while ((row = mysql_fetch_row(res)) != NULL){
   		unsigned long *lengths;
   		lengths = mysql_fetch_lengths(res);
		int i;
   		size_t num_fields = mysql_num_fields(res);
		for(i = 0; i < num_fields; i++) {
       			g_sprintf(hostname,"%.*s", (int) lengths[i],row[i] ? row[i] : "NULL");
			if (hostname != NULL) {
				char* temp = base+ (len*size);
				strcpy(temp,hostname);
				len++;
			}
   		}
	}
	mysql_free_result(res);
	g_free(hostname);
	mysql_close(mysql);
	mysql=NULL;
}
void update_daemon_sql(){
	char *path = get_setting_str(VIDEO_ROOT);
	char* name = NULL;
	if (path != NULL){
	#ifndef _WIN32
		struct utsname uts;
		uname( &uts );
		name = g_malloc(strlen(uts.nodename)+2);
	       	int len = strlen(uts.nodename);
		char* p;
		p=name;
		for (int i = 0; i<len; i++){*p++ = uts.nodename[i];}
	#else
		char hn[500 + 1];
		DWORD dwLen = 500;
		GetComputerName(hn, &dwLen);
		name = g_malloc(strlen(hn)+2);
		int len = strlen(hn);
		char* p;
		p=name;
		for (int i = 0; i<len; i++){*p++=hn[i];}
		*p='\0';

	#endif
		char* query =(char *)g_malloc(1024);
		g_sprintf(query,"INSERT INTO daemonRegister (hostname,pathToRoot) VALUES (\"%s\",\"%s\") ON DUPLICATE KEY UPDATE hostname=\"%s\",pathToRoot=\"%s\";",name,path,name,path);
		sql_exec_quary(query);
		g_free(query);
	}
}
#endif //#ifdef _SQL
