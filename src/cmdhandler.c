#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/utsname.h>
#include "dbus.h"
#include "sql.h"
#include "cmdhandler.h"

char* get_next_host(void* base,size_t size, size_t next){
	return base+(next*size);
}

void get_next_cmd() {
	char* cmd;
	char* cmdTxt;
	char* source;
	int cmdID;
	struct utsname uts;
	uname( &uts );
	get_next_cmd_from_sql(uts.nodename,&cmdID,&cmd,&cmdTxt,&source);
	#ifdef _DEBUG
		printf("ID:%i\ncmd:%s\ncmdTxt:%s\n",cmdID,cmd,cmdTxt);
	#endif
	if (!(strcmp(cmd," "))){
		return;;
	}
	if (!(strcmp(cmd,"STRB"))) {
		printf("Warning:STRB is deprecated");
		delete_from_cmdQueue(cmdID);
	} else if (!(strcmp(cmd,"SPRB"))) {
		printf("Warning:SPRB is deprecated");
		delete_from_cmdQueue(cmdID);
	} else if (!(strcmp(cmd,"NEXTRB"))) {
		if(send_command_to_rhythmbox("next"))
		{
			delete_from_cmdQueue(cmdID);
		} else {
			printf("Could not Next\n");
			delete_from_cmdQueue(cmdID);
		}
	} else if (!(strcmp(cmd,"BACKRB"))) {
                if(send_command_to_rhythmbox("previous"))  
                {
                        delete_from_cmdQueue(cmdID);
                } else {
                        printf("Could not BACK\n");
			delete_from_cmdQueue(cmdID);
                }
	} else if (!(strcmp(cmd,"PLAYRB"))) {
		if(send_command_to_rhythmbox_with_argument("playPause",G_TYPE_BOOLEAN,"TRUE"))  
                {
                        delete_from_cmdQueue(cmdID);
                } else {
                        printf("Could not PLAY\n");
			delete_from_cmdQueue(cmdID);
                }
	} else if (!(strcmp(cmd,"STOPRB"))) {
               	if(send_command_to_rhythmbox_with_argument("playPause",G_TYPE_BOOLEAN,"TRUE")) 
		{
                        delete_from_cmdQueue(cmdID);
                } else {
                        printf("Could not STOP\n");
			delete_from_cmdQueue(cmdID);
                }
	} else {
		printf("Command Not Recognized: ::%s::\n",cmd);
		delete_from_cmdQueue(cmdID);
	}
	
}


