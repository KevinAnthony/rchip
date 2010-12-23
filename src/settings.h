#ifndef SETTINGS_H
#define SETTINGS_H

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>

#ifdef _WIN32
	#define XMLFILE PREFIX"/share/settings.xml"
#else
	#define XMLFILE PREFIX"/share/madsci/settings.xml"
#endif

char* getsetting(char*);
char* find_element_names(xmlNode*,char*);
int xml_file_exists();
int new_xml_file();

#endif
