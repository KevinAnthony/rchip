#ifdef _WIN32
	#include 	<winsock.h>
	#include        <mysql.h>
	#include 	<windows.h>
#else
	#include	<mysql/mysql.h>
	#include 	<sys/utsname.h>
#endif
#include 	<glib.h>
#include	<string.h>
#include	<stdio.h>
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
		#ifndef _SILENT
			printf("Error init %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
		#endif
		return FALSE;
	}
	return TRUE;
}

void sql_exec_quary (char* query) {
	sql_init();
	#ifdef _DEBUG 
		printf("Query:%s\n",query);	
	#endif
	if (!sql_init()){ return; }
	if (mysql_real_connect(mysql,HOSTNAME,USERNAME,PASSWORD,DATABASE,0,NULL,0) == NULL) {
		#ifndef _SILENT	
			printf("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
		#endif
		mysql_close(mysql);
		mysql=NULL;
		printf("Returning because real_connect failed in sql_exec_quary");
		return;
	}
	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){
		#ifndef _SILENT
			printf("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
		#endif
		mysql_close(mysql);
		mysql=NULL;
		printf("Returning because real_query failed in sql_exec_quary");
		return;
	}
	mysql_close(mysql);
	mysql=NULL;
}

void get_next_cmd_from_sql(char *hostname,int* cmdID,char** cmd,char** cmdTxt, char** source) {
	char *query;
	query = (char*) malloc (sizeof("select * from cmdQueue where dest_hostname = '")+sizeof(hostname)+sizeof("';")+5);
	sprintf(query,"select * from cmdQueue where dest_hostname = '%s';",hostname);
	MYSQL_RES *res;
        MYSQL_ROW row;
	#ifdef _DEBUG 
                printf("Next Cmd Query:::: %s\n",query);
        #endif	
        if (!sql_init()){ return; }
	if (mysql_real_connect(mysql,HOSTNAME,USERNAME,PASSWORD,DATABASE,0,NULL,0) == NULL) {
                #ifndef _SILENT 
                        printf("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
                #endif
		mysql_close(mysql);
		mysql=NULL;
		printf("Returning because real_connect failed in get_next_cmd_from_sql");
                return;
        } 
        if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){
                #ifndef _SILENT
                        printf("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
                #endif
		mysql_close(mysql);
		mysql=NULL;
		printf("Returning because real_query failed in get_next_cmd_from_sql");
                return; 
        }

	res = mysql_use_result(mysql);
	if (res == NULL) { 
		mysql_close(mysql);
		mysql=NULL;
		//mysql_free_result(res);
		printf("Returning because res == NULL in get_next_cmd_from_sql");
		return;
	}
	if ((row = mysql_fetch_row(res))) {
	
		*cmd = (char*)malloc(1024);
		*cmdTxt = (char*)malloc(1024);
	        *source = (char*)malloc(1024);
		*cmdID = atoi(row[0] ? row[0] : NULL);
		sprintf(*cmd,"%s",row[2] ? row[2] : NULL);
		sprintf(*cmdTxt,"%s",row[3] ? row[3] : NULL);
		sprintf(*source,"%s",row[4] ? row[4] : NULL);
	} else {
		*cmd = " ";
		*cmdTxt = " ";
		*source = " ";
		*cmdID = 0;
	}
	//mysql_free_result(res);
	mysql_close(mysql);
	mysql=NULL;
	free(query);
}

void delete_from_cmdQueue(int cmdID) {
	char *delquery;
        delquery = (char*) malloc (500);
        sprintf(delquery,"delete from cmdQueue where id =%i;",cmdID);
	#ifdef _DEBUG 
                printf("Next Cmd Query:::: %s\n",delquery);
        #endif
        if (!sql_init()){ return; }
	if (mysql_real_connect(mysql,HOSTNAME,USERNAME,PASSWORD,DATABASE,0,NULL,0) == NULL) {
                #ifndef _SILENT 
                        printf("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
                #endif
		mysql_close(mysql);
		mysql=NULL;
		printf("Returning because real_connect failed in delete_from_cmdQueue");
                return;
        }
        if(mysql_real_query(mysql,delquery,(unsigned int)strlen(delquery))){
                #ifndef _SILENT
                        printf("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
                #endif
		printf("Returning because real_query failed in delete_from_cmdQueue");
		mysql_close(mysql);
		mysql=NULL;
                return;
        }
	mysql_close(mysql);
	mysql=NULL;
	free(delquery);
}

char* get_registered_devices_message(){
	char* query = "select source_hostname from messageRegister where active = \"TRUE\"";
	MYSQL_RES *res;
        MYSQL_ROW row;
	char* returnval = (char*) malloc(1024);
	if (!sql_init()){ return NULL; }
	if (mysql_real_connect(mysql,HOSTNAME,USERNAME,PASSWORD,DATABASE,0,NULL,0) == NULL) {
                #ifndef _SILENT 
                        printf("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
                #endif
		mysql_close(mysql);
		mysql=NULL;
		printf("Returning because real_connect failed in get_registered_devices_message");
                return NULL;
        }
        if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){
                #ifndef _SILENT
                        printf("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
                #endif
		mysql_close(mysql);
		mysql=NULL;
		printf("Returning because real_query failed in get_registered_devices_message");
                return NULL;
        }

        res = mysql_use_result(mysql);
        if (res == NULL) {
		//mysql_free_result(res);
		mysql_close(mysql);
		mysql=NULL;
		printf("Returning because res == NULL in get_registered_devices_message");
                return NULL;
        }
	row=mysql_fetch_row(res);
	sprintf(returnval,"%s",row[0]);
	while ((row = mysql_fetch_row(res))) {
		returnval=strcat(returnval,row[0]);
	}
	//mysql_free_result(res);
        mysql_close(mysql);
	mysql=NULL;
	return returnval;
}

size_t get_nelem(){
	char* query = "select hostname from remoteActiveDevices where active = \"TRUE\"";
        MYSQL_RES *res;
	if (!sql_init()){ return -1; }
	if (mysql_real_connect(mysql,HOSTNAME,USERNAME,PASSWORD,DATABASE,0,NULL,0) == NULL) {
                #ifndef _SILENT 
                        printf("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
                #endif
		printf("Returning because real_connect failed in get_nelem");
		mysql_close(mysql);
		mysql=NULL;
                return -1;
        }

	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){
                #ifndef _SILENT
                        printf("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
                #endif
		mysql_close(mysql);
		mysql=NULL;
		printf("Returning because real_query failed in get_nelem");
                return -1;
        }
        res = mysql_store_result(mysql);
	size_t retval = (int)mysql_num_rows(res);
	//mysql_free_result(res);
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
        char* hostname = (char*) malloc(HOSTNAME_MAX_SIZE);
	if (!sql_init()){ return; }
        if (mysql_real_connect(mysql,HOSTNAME,USERNAME,PASSWORD,DATABASE,0,NULL,0) == NULL) {
                #ifndef _SILENT 
                        printf("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
                #endif
                mysql_close(mysql);
		mysql=NULL;
		printf("Returning because real_connect failed in get_active_device");
		return;
        }
	
	
	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){
                #ifndef _SILENT
                        printf("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
                #endif
		mysql_close(mysql);
		mysql=NULL;
                printf("Returning because real_query failed in get_active_device");
		return;
        }
        res = mysql_use_result(mysql);

	if (res == NULL) {
		//mysql_free_result(res);
		mysql_close(mysql);
		mysql=NULL;
                printf("Returning because res == NULL in get_active_device");
		return;
        }
	size_t len = 0;	
	
	while ((row = mysql_fetch_row(res)) != NULL){
   		unsigned long *lengths;
   		lengths = mysql_fetch_lengths(res);
		int i;
   		size_t num_fields = mysql_num_fields(res);
		for(i = 0; i < num_fields; i++) {
       			sprintf(hostname,"%.*s", (int) lengths[i],row[i] ? row[i] : "NULL");
			if (hostname != NULL) {
				char* temp = base+ (len*size);
				strcpy(temp,hostname);
				len++;
			}
   		}
	}
	//mysql_free_result(res);
	free(hostname);
        mysql_close(mysql);
	mysql=NULL;
}
void update_daemon_sql(){
	char *path = getsetting("pathToRoot");
        char* name = NULL;
	if (path != NULL){
	#ifndef _WIN32
                struct utsname uts;
                uname( &uts );
		name = malloc(strlen(uts.nodename)+2);
               	int len = strlen(uts.nodename);
		char* p;
		p=name;
        	for (int i = 0; i<len; i++){*p++ = uts.nodename[i];}
	#else
                char hn[500 + 1];
                DWORD dwLen = 500;
                GetComputerName(hn, &dwLen);
		name = malloc(strlen(hn)+2);
		int len = strlen(hn);
                char* p;
                p=name;
                for (int i = 0; i<len; i++){*p++=hn[i];}
		*p='\0';

        #endif
		char* query =(char *)malloc(1024);
		sprintf(query,"INSERT INTO daemonRegister (hostname,pathToRoot) VALUES (\"%s\",\"%s\") ON DUPLICATE KEY UPDATE hostname=\"%s\",pathToRoot=\"%s\";",name,path,name,path);
		sql_exec_quary(query);
		free(query);
        }
}

