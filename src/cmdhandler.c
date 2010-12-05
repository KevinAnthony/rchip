#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/utsname.h>
#include "dbus.h"
#include "sql.h"
#include "cmdhandler.h"

hn_list *hosts_head = NULL;
hn_list *hosts_cur = NULL;

gboolean add_to_hosts(char* item) {
	hn_list *temp;
	hn_list *last;
	if (hosts_head == NULL) {
		temp = (hn_list*) malloc(sizeof(hn_list));
		temp->hostname = item;
		temp->next = NULL;
		hosts_head = temp;
	} else {
		temp = hosts_head;
		while(temp->next != NULL)
		{
			if (temp->hostname == item) {
				return FALSE;
			}
			temp=temp->next; 
		}
		if (temp->hostname == item) {
                	return FALSE;
                }
		last = (hn_list*) malloc(sizeof(hn_list));
		last->hostname = item;
		last->next = NULL;
		temp->next = last;
	}
	return TRUE;
}

gboolean delete_from_hosts(char* item){
	printf("Deleteing: %s\n",item);
	if (hosts_head == NULL) {
		return FALSE;
	} else {
		hn_list *old;
		hn_list *temp = hosts_head;
		int found = 0;
		while (temp != NULL) {
			if (temp->hostname == item) {
				if (temp == hosts_head) {
					hosts_head = temp->next;
				} else {
					old->next=temp->next;
				}
				free(temp);
				found++;
			} else {
				old = temp;
				temp=temp->next;
			}
		}
		if (found == 0) {
			return FALSE;
		}
	}
	return TRUE;
}

char* get_next_host(){
	if (hosts_head == NULL) {
		return NULL;
	}
	if (hosts_cur == NULL) {
		hosts_cur = hosts_head;
	} else {
		hosts_cur = hosts_cur->next;
	}
	if (hosts_cur == NULL){
		return NULL;
	} else {
		return hosts_cur->hostname;
	}
}

void print_list() {
	hn_list *node = hosts_head;
	while(node) {
		printf("hostname: %s\n",node->hostname);
		node = node->next;
	}
	printf ("\n");
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
		printf("STRB\n");
		add_to_hosts(source);
		delete_from_cmdQueue(cmdID);
	} else if (!(strcmp(cmd,"SPRB"))) {
		delete_from_hosts(source);
		delete_from_cmdQueue(cmdID);
	} else if (!(strcmp(cmd,"NEXT"))) {
		if(send_command_to_player("next"))
		{
			delete_from_cmdQueue(cmdID);
		} else {
			printf("Could not Next\n");
			delete_from_cmdQueue(cmdID);
		}
	} else if (!(strcmp(cmd,"BACK"))) {
                if(send_command_to_player("previous"))  
                {
                        delete_from_cmdQueue(cmdID);
                } else {
                        printf("Could not BACK\n");
			delete_from_cmdQueue(cmdID);
                }
	} else if (!(strcmp(cmd,"PLAY"))) {
                printf("umm, playing...");
		if(send_command_to_player_with_argument("playPause",G_TYPE_BOOLEAN,"TRUE"))  
                {
                        delete_from_cmdQueue(cmdID);
                } else {
                        printf("Could not PLAY\n");
			delete_from_cmdQueue(cmdID);
                }
	} else if (!(strcmp(cmd,"STOP"))) {
               	if(send_command_to_player_with_argument("playPause",G_TYPE_BOOLEAN,"TRUE")) 
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


