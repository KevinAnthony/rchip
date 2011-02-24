#include <config.h>

#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#ifndef _WIN32
	#include <sys/utsname.h>
	#include "dbus.h"
#else
	#include <windows.h>
	#include <process.h>
	#define myerrno WSAGetLastError()
#endif
#include "sql.h"
#include "cmdhandler.h"
#include "settings.h"
#include "utils.h"

char* get_next_host(void* base,size_t size, size_t next){
	return base+(next*size);
}

void get_next_cmd() {
	char* cmd;
	char* cmdTxt;
	char* source;
	int cmdID;
	#ifndef _WIN32
		struct utsname uts;
		uname( &uts );
		#ifdef _SQL
		get_next_cmd_from_sql(uts.nodename,&cmdID,&cmd,&cmdTxt,&source);
		#endif
	#else
		//char* smplayerLocation = "\"c:\\program files\\smplayer\\smplayer.exe\"";
		char hn[500 + 1];
		DWORD dwLen = 500;
		GetComputerName(hn, &dwLen);
		#ifdef _SQL
		get_next_cmd_from_sql(hn,&cmdID,&cmd,&cmdTxt,&source);
		#endif
	#endif
	#if VERBOSE >= 4
		printf("ID:%i\ncmd:%s\ncmdTxt:%s\n",cmdID,cmd,cmdTxt);
	#endif
	if (!(strcmp(cmd," "))){
		return;
	}
	if (!(strcmp(cmd,"STRB"))) {
		#if VERBOSE >= 1
		printf("Warning:STRB is deprecated");
		#endif
		#ifdef _SQL
		delete_from_cmdQueue(cmdID);
		#endif
	} else if (!(strcmp(cmd,"SPRB"))) {
		#if VERBOSE >= 1
		printf("Warning:SPRB is deprecated");
		#endif
		#ifdef _SQL
		delete_from_cmdQueue(cmdID);
		#endif
	} else if (!(strcmp(cmd,"NEXTRB"))) {
		#ifndef _WIN32
			if(send_command_to_rhythmbox("next"))
			{
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			} else {
				#if VERBOSE >= 2
				printf("Could not Next\n");
				#endif
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
			}
		#else
			#ifdef _SQL
			delete_from_cmdQueue(cmdID);
			#endif
		#endif
	} else if (!(strcmp(cmd,"BACKRB"))) {
		#ifndef _WIN32
                	if(send_command_to_rhythmbox("previous"))  
                	{
				#ifdef _SQL
                	        delete_from_cmdQueue(cmdID);
				#endif
                	} else {
				#if VERBOSE >= 2
                	        printf("Could not BACK\n");
				#endif
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
                	}
		#else
			#ifdef _SQL
			delete_from_cmdQueue(cmdID);
			#endif
		#endif
	} else if (!(strcmp(cmd,"PLAYRB"))) {
		#ifndef _WIN32
			if(send_command_to_rhythmbox_with_argument("playPause",G_TYPE_BOOLEAN,"TRUE"))  
	                {
				#ifdef _SQL
	                        delete_from_cmdQueue(cmdID);
				#endif
	                } else {
				#if VERBOSE >= 2
       		                printf("Could not PLAY\n");
				#endif
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
                	}
		#else
			#ifdef _SQL
			delete_from_cmdQueue(cmdID);
			#endif
		#endif
	} else if (!(strcmp(cmd,"STOPRB"))) {
        	#ifndef _WIN32
		       	if(send_command_to_rhythmbox_with_argument("playPause",G_TYPE_BOOLEAN,"TRUE")) 
			{
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
        	        } else {
				#if VERBOSE >= 2
        	                printf("Could not STOP\n");
				#endif
				#ifdef _SQL
				delete_from_cmdQueue(cmdID);
				#endif
        	        }
		#else
			#ifdef _SQL
			delete_fvirom_cmdQueue(cmdID);
			#endif
		#endif
	} else if (!(strcmp(cmd,"PLAYSM"))) {
		if (system("smplayer -send-action play")) {
			#ifdef _SQL
			delete_from_cmdQueue(cmdID);
			#endif
		} else {
                	#if VERBOSE >= 2
			printf("Could not Play SMplayer\n");
			#endif
			#ifdef _SQL
			delete_from_cmdQueue(cmdID);
			#endif
                }

	} else if (!(strcmp(cmd,"STOPSM"))) {
               	if (system("smplayer -send-action stop")) {
			#ifdef _SQL
			delete_from_cmdQueue(cmdID);
			#endif
                } else {
			#if VERBOSE >= 2
                        printf("Could not Stop SMplayer\n");
			#endif
                        #ifdef _SQL
			delete_from_cmdQueue(cmdID);
			#endif
                }
	} else if (!(strcmp(cmd,"PAUSESM"))) {
       	 	if (system("smplayer -send-action pause")) {
                        #ifdef _SQL
			delete_from_cmdQueue(cmdID);	
			#endif
                } else {
			#if VERBOSE >= 2
                        printf("Could not Pause SMplayer\n");
			#endif
			#ifdef _SQL
                        delete_from_cmdQueue(cmdID);
			#endif
                }

       } else if (!(strcmp(cmd,"SKIPFSM"))) {
               	if (system("smplayer -send-action forward1")) {
			#ifdef _SQL
                        delete_from_cmdQueue(cmdID);
			#endif
                } else {
			#if VERBOSE >= 2
                        printf("Could not Skip Foward SMplayer\n");
			#endif
			#ifdef _SQL
                        delete_from_cmdQueue(cmdID);
			#endif
                }
	} else if (!(strcmp(cmd,"SKIPBSM"))) {
               	if (system("smplayer -send-action rewind1")) {
                	#ifdef _SQL
                        delete_from_cmdQueue(cmdID);
                        #endif
		} else {
			#if VERBOSE >= 2
                        printf("Could not Skip Backwards SMplayer\n");
			#endif
                        #ifdef _SQL
                        delete_from_cmdQueue(cmdID);
                        #endif
                }
	} else if (!(strcmp(cmd,"FULLONSM"))) {
               	if (system("smplayer -send-action fullscreen")) {
                        #ifdef _SQL
                        delete_from_cmdQueue(cmdID);
                        #endif
                } else {
			#if VERBOSE >= 2
                        printf("Could not Fullscreen SMplayer\n");
			#endif
                        #ifdef _SQL
                        delete_from_cmdQueue(cmdID);
                        #endif
                }
	} else if (!(strcmp(cmd,"FULLOFFSM"))) {
               	if (system("smplayer -send-action exit-fullscreen")) {
                        #ifdef _SQL
                        delete_from_cmdQueue(cmdID);
                        #endif
                } else {
			#if VERBOSE >= 2
                        printf("Could not Fullscreen off SMplayer\n");
			#endif
                        #ifdef _SQL
                        delete_from_cmdQueue(cmdID);
                        #endif
                }
	} else if (!(strcmp(cmd,"OPENSM"))) {
		
		char* command;
		#ifdef _WIN32
			char* filename = cmdTxt;
			char* cmd= "smplayer.exe";
			char* path="c:\\Program Files\\smplayer\\smplayer.exe";
			command = malloc(1);
			while (*filename++ != '\0') {if (*filename == '/') { *filename = '\\';} }
			if(spawnl(P_NOWAIT, path ,cmd ,cmdTxt,NULL) >= 0) {
		#else
			char* text = "smplayer ";
			command = malloc(strlen(cmdTxt)+strlen(text)+1);
			sprintf(command,"%s%s&",text,cmdTxt);
			if (system(command)) {
		#endif
			#ifdef _SQL
		       	delete_from_cmdQueue(cmdID);
			#endif
                } else {
			#if VERBOSE >= 2
                       	printf("Could not open SMplayer\n");
			#endif
			#ifdef _WIN32
				#if VERBOSE >= 2
				printf("Last error: %i\n",myerrno);
				#endif
			#endif
			#ifdef _SQL
                       	delete_from_cmdQueue(cmdID);
			#endif
                }
		free(command);	
	} else if (!(strcmp(cmd,"MUTESM"))) {
               	if (system("smplayer -send-action mute")) {
			#ifdef _SQL
                        delete_from_cmdQueue(cmdID);
			#endif
                } else {
			#if VERBOSE >= 2
                        printf("Could not Play SMplayer\n");
			#endif
			#ifdef _SQL
                        delete_from_cmdQueue(cmdID);
			#endif
                }
	} else if (!(strcmp(cmd,"QUITSM"))) {
               	if (system("smplayer -send-action quit")) {
			#ifdef _SQL
                        delete_from_cmdQueue(cmdID);
			#endif
                } else {
                        #if VERBOSE >= 2
			printf("Could not Play SMplayer\n");
			#endif
			#ifdef _SQL
                        delete_from_cmdQueue(cmdID);
			#endif
                }
 	} else {
		#if VERBOSE >= 2
		printf("Command Not Recognized: ::%s::\n",cmd);
		#endif
		#ifdef _SQL
		delete_from_cmdQueue(cmdID);
		#endif
	}
	free(cmd);
	free(cmdTxt);
	free(source);
}
void send_cmd(char* cmd, char* cmdTxt) {
	#ifdef _SQL
	size_t nelem;
	size_t size;
	char* base;
	size=get_size();
        nelem=get_nelem();
        base = calloc(nelem,size);
        get_active_devices(base,size,nelem);
	char* hostname = malloc(size);
                for (int i = 0; i < nelem; i++) {
                        char* elem = base+(i*size);
                        strcpy(hostname,elem);
                        char* query = (char *)malloc(1024);
                        sprintf(query,"INSERT INTO cmdQueue (command,cmdText,source_hostname,dest_hostname) values (\"%s\",\"%s\",\"%s\",\"%s\");",cmd,cmdTxt,"Tomoya",hostname);
			sql_exec_quary(query);
                        free(query);
                }
	free(hostname);
	#endif
}
