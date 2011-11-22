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

#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include <glib/gprintf.h>
#include "utils.h"

char* replace_str(char* str, char* orig, char* rep){
	static char buffer[sizeof(orig)+sizeof(rep)+sizeof(str)+1];
	char* p;
	if(!(p = g_strstr_len(str,-1, orig)))  // Is 'orig' even in 'str'?
    		return str;
  	strncpy(buffer, str, p-str); // Copy characters from 'str' start to 'orig' st$
  	buffer[p-str] = '\0';
  	g_sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));
  	return buffer;
}

void init_hostname ( void ){
	Hosts = malloc(sizeof(hostname*));
	Hosts->add = *add;
	Hosts->delete = *delete;
	Hosts->find = *find;
	Hosts->data=NULL;
}

void add (char* data){
	if (!Hosts->find(data)){
		hostname_node* new_node = malloc(sizeof(hostname_node*));
		new_node->hostname = g_strdup(data);
		new_node->next = NULL;
		if (Hosts->data == NULL){
			Hosts->data = new_node;
			return;
		}
		hostname_node* hostname_p = Hosts->data;
		while (hostname_p->next != NULL){next_hostname(hostname_p);}
		hostname_p->next = new_node;
	}
}

void delete (char* data){
	if (Hosts->data == NULL){
		return;
	}
	hostname_node* cur = Hosts->data;
	hostname_node* prev = NULL;
	while (cur != NULL){
		if (g_strcmp0(cur->hostname,data) == 0){
			if (cur == Hosts->data){
				Hosts->data=cur->next;
				g_free(cur->hostname);
				g_free(cur);
				cur = Hosts->data;
			} else {
				prev->next = cur->next;
				g_free(cur->hostname);
				g_free(cur);
				cur = prev->next;
			}
		}
		if (prev == NULL){
			prev = Hosts->data;
		} else {
			next_hostname(prev);
		}
		next_hostname(cur);
	}
}

int find (char* token){
	
	hostname_node* hostname_p = Hosts->data;
	for_each_hostname(hostname_p){
		if (g_strcmp0(hostname_p->hostname, token) == 0){
			return 1;
		}
	}
	return 0;
}
