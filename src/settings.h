#ifndef SETTINGS_H
#define SETTINGS_H

#include <string.h>

#ifdef _WIN32
	#include <libxml/parser.h>
	#include <libxml/tree.h>
	#define XMLFILE PREFIX"/share/settings.xml"
	char* getsetting(char*);
	char* find_element_names(xmlNode*,char*);
	int xml_file_exists();
	int new_xml_file();

#else
	#include <glib.h>
	#include <gconf/gconf-client.h>
	#include <stdio.h>
	#define SERVICE_GCONF_ROOT "/apps/noside/msdaemon"
	#define VIDEO_ROOT "/videoroot"
	#define SERVICE_KEY_PATH_TO_VIDEO_ROOT \
	        SERVICE_GCONF_ROOT "/videoroot"
	
	void keyChangeCallback(GConfClient*,guint,GConfEntry*,gpointer);
	int registerCallback( void (*) (void),gchar*);
	int unregisterCallback( void (*) (gchar*));
	int callbackRegisteredFunctions(gchar*);
	char* getsetting( gchar*);
	void populate_defaults(GConfClient*);
	int settings_init();

#endif // # ifdef _WIN32
#endif
