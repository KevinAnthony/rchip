#include 	<mysql/mysql.h> 
#include 	<glib.h>
#include	<string.h>
#include	<stdio.h>
#include	<stdlib.h>
#include 	"sql.h"

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
	if (mysql_real_connect(mysql,HOSTNAME,USERNAME,PASSWORD,DATABASE,0,NULL,0) == NULL) {
		#ifndef _SILENT	
			printf("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
		#endif
		return;
	}
	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){
		#ifndef _SILENT
			printf("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
		#endif
		return;
	}
	mysql_close(mysql);
}

void get_next_cmd_from_sql(char *hostname,int* cmdID,char** cmd,char** cmdTxt, char** source) {
	char *query;
	query = (char*) malloc (sizeof("select * from cmdQueue where dest_hostname = '")+sizeof(hostname)+sizeof("';"));
	sprintf(query,"select * from cmdQueue where dest_hostname = '%s';",hostname);
        MYSQL_RES *res;
        MYSQL_ROW row;
	#ifdef _DEBUG 
                printf("Next Cmd Query:::: %s\n",query);
        #endif	
	sql_init();	
        if (mysql_real_connect(mysql,HOSTNAME,USERNAME,PASSWORD,DATABASE,0,NULL,0) == NULL) {
                #ifndef _SILENT 
                        printf("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
                #endif
                return;
        } 
        if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){
                #ifndef _SILENT
                        printf("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
                #endif
                return; 
        }

	res = mysql_use_result(mysql);
	if (res == NULL) { 
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
	mysql_free_result(res);
	mysql_close(mysql);
}

void delete_from_cmdQueue(int cmdID) {
	char *query;
        query = (char*) malloc (sizeof("delete from cmdQueue where id =")+sizeof(cmdID)+sizeof(";"));
        sprintf(query,"delete from cmdQueue where id =%i; ;",cmdID);
        #ifdef _DEBUG 
                printf("Next Cmd Query:::: %s\n",query);
        #endif
        sql_init();
        if (mysql_real_connect(mysql,HOSTNAME,USERNAME,PASSWORD,DATABASE,0,NULL,0) == NULL) {
                #ifndef _SILENT 
                        printf("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
                #endif
                return;
        }
        if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){
                #ifndef _SILENT
                        printf("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
                #endif
                return;
        }
}

char* get_registered_devices_message(){
	char* query = "select source_hostname from messageRegister where active = \"TRUE\"";
	MYSQL_RES *res;
        MYSQL_ROW row;
	char* returnval = (char*) malloc(1024);
	if (mysql_real_connect(mysql,HOSTNAME,USERNAME,PASSWORD,DATABASE,0,NULL,0) == NULL) {
                #ifndef _SILENT 
                        printf("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
                #endif
                return NULL;
        }
        if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){
                #ifndef _SILENT
                        printf("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
                #endif
                return NULL;
        }

        res = mysql_use_result(mysql);
        if (res == NULL) {
                return NULL;
        }
	row=mysql_fetch_row(res);
	sprintf(returnval,"%s",row[0]);
	while ((row = mysql_fetch_row(res))) {
		returnval=strcat(returnval,row[0]);
	}
	return returnval;
}

size_t get_nelem(){
	char* query = "select hostname from remoteActiveDevices where active = \"TRUE\"";
        MYSQL_RES *res;
	if (mysql_real_connect(mysql,HOSTNAME,USERNAME,PASSWORD,DATABASE,0,NULL,0) == NULL) {
                #ifndef _SILENT 
                        printf("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
                #endif
                return -1;
        }

	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){
                #ifndef _SILENT
                        printf("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
                #endif
                return -1;
        }
        res = mysql_store_result(mysql);
        return (int)mysql_num_rows(res);
}

size_t get_size(){
	return HOSTNAME_MAX_SIZE;
}

void get_active_devices(void* base, size_t size, size_t nelem)
{	
	char* query = "select hostname from remoteActiveDevices where active = \"TRUE\"";
        MYSQL_RES *res;
        MYSQL_ROW row;
        char* hostname = (char*) malloc(HOSTNAME_MAX_SIZE);
        if (mysql_real_connect(mysql,HOSTNAME,USERNAME,PASSWORD,DATABASE,0,NULL,0) == NULL) {
                #ifndef _SILENT 
                        printf("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
                #endif
                return;
        }
        if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){
                #ifndef _SILENT
                        printf("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
                #endif
                return;
        }
	res = mysql_store_result(mysql);
	if(mysql_real_query(mysql,query,(unsigned int)strlen(query))){
                #ifndef _SILENT
                        printf("Error %u: %s\n",mysql_errno(mysql),mysql_error(mysql));
                #endif
                return;
        }
        res = mysql_use_result(mysql);

	if (res == NULL) {
                return;
        }
	size_t len = 0;	
	size_t num_fields = mysql_num_fields(res);
		
	while ((row = mysql_fetch_row(res))){
   		unsigned long *lengths;
   		lengths = mysql_fetch_lengths(res);
		int i;
   		for(i = 0; i < num_fields; i++) {
       			sprintf(hostname,"%.*s", (int) lengths[i],row[i] ? row[i] : "NULL");
			if (hostname != NULL) {
				char* temp = base+ (len*size);
				strcpy(temp,hostname);
				len++;
			}
   		}
	}
}

