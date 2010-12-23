#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "showlist.h"
#include "cmdhandler.h"
#include "settings.h"

void add_file_to_playqueue(char* filepath){
	//here i really should somehow figure out if it's an anime, a live action, or something else
	// for now, it's all live action
	char *path = getsetting("pathToRoot");
        if (path != NULL){
		filepath = replace_str(filepath,path,"/mnt/raid/");
	}	
	#ifdef _WIN32
		if (strstr(filepath,"\\English\\Live_Action\\") != NULL) {
	#else
		if (strstr(filepath,"/English/Live_Action/") != NULL) {
	#endif
		send_cmd("ADDS",live_action(filepath));
	#ifdef _WIN32
		} else if (strstr(filepath,"\\Foreign\\Animeted\\") != NULL) {
	#else
		} else if (strstr(filepath,"/Foreign/Animeted/") != NULL) {
	#endif
		send_cmd("ADDS",anime(filepath));
	} else {
		send_cmd("ADDS",other(filepath));
	}

}

char* live_action(char* filepath){
	char* posOfLastSlash=filepath;
	int fnamelen=0;
        int lenOfName=0;
        int lenOfEpsNumber=0;
        int lenOfEpsName=0;
	int totallen=0;
	int totalex=0;
	char* ptr = filepath;
	char* sptr = filepath;
	for (; *ptr != '\0';ptr++) {
	#ifdef _WIN32
		if (*ptr == '\\') {
			totalex++;
	#else
		if (*ptr == '/') {
	#endif
			posOfLastSlash = ptr;
		}
		fnamelen++;
	}
	ptr = posOfLastSlash;
	ptr++;
	sptr=ptr;
	for (int i = 0; i < 3; i++){
		for (; *ptr!='.' ; ptr++){
			if (*ptr == '\0'){
				if (i == 2){ break; }
				//#ifndef _SILENT
					printf("Error: badly formed file name\n");
				//#endif
				return NULL;
			}
			if (i == 0){ lenOfName++; }
			else if (i == 1){ lenOfEpsNumber++; }
			else if (i == 2){ lenOfEpsName++; }
		}
		ptr++;
	}
	if (*ptr == '\0'){ lenOfEpsName = 0;}
	totallen=fnamelen+lenOfName+lenOfEpsNumber+lenOfEpsName+totalex;
	char* retval = malloc(totallen+5);
	ptr = retval;
	for (int i = 0; i < lenOfName; i++){
		*ptr++=*sptr++;
	}
	*ptr++='|';
	sptr++;
	for (int i = 0; i < lenOfEpsNumber; i++){
                *ptr++=*sptr++;
        }
        *ptr++='|';
        sptr++;
	for (int i = 0; i < lenOfEpsName; i++){
                *ptr++=*sptr++;
        }
        *ptr++='|';
        sptr=filepath;
	for (int i = 0; i < fnamelen; i++){
	#ifdef _WIN32
		if (*sptr == '\\') {
			*ptr++='\\';
		}
	#endif
		*ptr++=*sptr++;
	}
	*ptr='\0';
	printf("%s\n",retval);
	return retval;
}

char* anime(char* filepath){
	if (strstr(filepath,"One_Piece") != NULL) {
		return std_anime(filepath,"One_Piece");
        } else if (strstr(filepath,"Fairy_Tail") != NULL) {
                return std_anime(filepath,"Fairy_Tail");
        } else {
                return other(filepath);
        }


}

char* other(char* filepath){
	char* posOfLastSlash=filepath;
        int fnamelen=0;
        int lenOfName=0;
        int totallen=0;
        int totalex=0;
	char* ptr = filepath;
        char* sptr = filepath;
	for (; *ptr != '\0';ptr++) {
	#ifdef _WIN32
		if (*ptr == '\\') {
			totalex++;
	#else
		if (*ptr == '/') {
	#endif
                        posOfLastSlash = ptr;
                }
                fnamelen++;
        }
	ptr = posOfLastSlash +1;
        for (; *ptr != '\0'; ptr++){
        	lenOfName++;
	}
        totallen=fnamelen+lenOfName+5+totalex;
        char* retval = malloc(totallen+5);
        ptr=retval;
	sptr = posOfLastSlash+1;
	char* other = "Other";
	for (int i = 0; i < 5; i++) {
		*ptr++=*other++;
	}
	*ptr++='|';
        for (int i = 0; i < lenOfName; i++){
                *ptr++=*sptr++;
        }
        *ptr++='|';
        *ptr++='|';
        sptr=filepath;
	for (int i = 0; i < fnamelen; i++){
	#ifdef _WIN32
		if (*sptr == '\\') {
			*ptr++='\\';
		}
	#endif
		*ptr++=*sptr++;
        }
        *ptr='\0';
        return retval;

}

char* std_anime(char* filepath,char* name){
	char* posOfLastSlash=filepath;
        int fnamelen=0;
        int lenOfName=strlen(name);
        int lenOfEpsNumber=0;
        int lenOfSubgroup=0;
        int totallen=0;
	int totalex=0;
        char* ptr = filepath;
        char* sptr = filepath;
        for (; *ptr != '\0';ptr++) {
        #ifdef _WIN32
		if (*ptr == '\\') {
			totalex++;
	#else
		if (*ptr == '/') {
	#endif
			posOfLastSlash = ptr;
                }
                fnamelen++;
        }
        ptr = posOfLastSlash+1;
        ptr = ptr+lenOfName+1;
        sptr=ptr;
        for (; *ptr!='_' ; ptr++){
        	if (*ptr == '\0'){
                        //#ifndef _SILENT
                        	printf("Error: badly formed file name\n");
                       	//#endif
                        return NULL;
             	}
		lenOfEpsNumber++;
        }
        ptr++;
	ptr++;
        for (; *ptr!=']' ; ptr++){
               if (*ptr == '\0'){
                       //#ifndef _SILENT
                               printf("Error: badly formed file name\n");
                       //#endif
                       return NULL;
               }
               lenOfSubgroup++;
       }

	if (*ptr == '\0'){ lenOfSubgroup = 0;}
        totallen=fnamelen+lenOfName+lenOfEpsNumber+lenOfSubgroup+totalex;
        char* retval = malloc(totallen+5);
        ptr = retval;
	sptr=name;
        for (int i = 0; i < lenOfName; i++){
                *ptr++=*sptr++;
        }
        *ptr++='|';
        sptr=posOfLastSlash+lenOfName+2;
        for (int i = 0; i < lenOfEpsNumber; i++){
                *ptr++=*sptr++;
        }
        *ptr++='|';
        while (*sptr++ != '['){ lenOfSubgroup--; }
	lenOfSubgroup++;	
        for (int i = 0; i < lenOfSubgroup; i++){
                *ptr++=*sptr++;
        }
        *ptr++='|';
        sptr=filepath;
	for (int i = 0; i < fnamelen; i++){
	#ifdef _WIN32
		if (*sptr == '\\') {
			*ptr++='\\';
		}
	#endif
	      	*ptr++=*sptr++;
        }
        *ptr='\0';
	return retval;

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
