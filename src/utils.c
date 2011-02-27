#include <config.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"


void init_playing_info_music(struct playing_info_music *pInfo){
	struct playing_info_music p = *pInfo;
	p.Artist = malloc(1024);
	p.Album = malloc(1024);
	p.Song = malloc(1024);
	*pInfo = p;
}

void free_playing_info_music(struct playing_info_music *pInfo){
	struct playing_info_music p = *pInfo;	
	//free(p.Artist);
	//free(p.Album);
	//free(p.Song);
	*pInfo = p;
}


char* replace_str(char* str, char* orig, char* rep){
	static char buffer[4096];
	char* p;
	if(!(p = strstr(str, orig)))  // Is 'orig' even in 'str'?
    		return str;
  	strncpy(buffer, str, p-str); // Copy characters from 'str' start to 'orig' st$
  	buffer[p-str] = '\0';
  	sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));
  	return buffer;
}
