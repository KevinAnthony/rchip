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

#include "settings.h"
#include <glib.h>
#include <glib/gprintf.h>
#ifdef _WIN32
#include <parser.h>
#include <tree.h>

#ifdef LIBXML_TREE_ENABLED
#ifdef LIBXML_OUTPUT_ENABLED

char* get_setting(char* settingname){
	
	if (!(xml_file_exists())){
		if(!(new_xml_file())){
			return NULL;
		}
	}
	xmlDoc *doc = NULL;
    	xmlNode *root_element = NULL;
	LIBXML_TEST_VERSION

	/*parse the file and get the DOM */
    	doc = xmlReadFile(XMLFILE, NULL, 0);

    	if (doc == NULL) {
		#if VERBOSE >= 1
		g_error("error: could not parse file %s\n", XMLFILE);
		#endif
    	}

	/*Get the root element node */
    	root_element = xmlDocGetRootElement(doc);

    	char* retval = find_element_names(root_element,settingname);

    	/*free the document */
    	xmlFreeDoc(doc);

    	xmlCleanupParser();
	return retval;
}

char* find_element_names(xmlNode * a_node,char* setting)
{
    	xmlNode *cur_node = NULL;
	cur_node = a_node->xmlChildrenNode;
	while (cur_node != NULL) {
		if (!(strcmp((char*)cur_node->name,setting))){
			return (char*) xmlNodeListGetString(NULL, cur_node->xmlChildrenNode, 1);
		}
	cur_node = cur_node->next;
	}
	return NULL;
}

int xml_file_exists()
{
	FILE * file = fopen(XMLFILE, "r");
	if (file != NULL){
		fclose(file);
		return 1;
    	}
    	return 0;
}

int new_xml_file() {
	xmlDocPtr doc = NULL;       /* document pointer */
    	xmlNodePtr root_node = NULL;
    	xmlDtdPtr dtd = NULL;       /* DTD pointer */

    	LIBXML_TEST_VERSION;

    	/* 
    	 * Creates a new document, a node and set it as a root node
    	 */
    	doc = xmlNewDoc(BAD_CAST "1.0");
    	root_node = xmlNewNode(NULL, BAD_CAST "settings");
    	xmlDocSetRootElement(doc, root_node);

    	/*
    	 * Creates a DTD declaration. Isn't mandatory. 
    	 */
    	dtd = xmlCreateIntSubset(doc, BAD_CAST "settings", NULL, BAD_CAST "msdeamon.dtdi");
    	xmlNewChild(root_node, NULL, BAD_CAST "pathToRoot",
			BAD_CAST "/mnt/raid/");
	
	
	 xmlSaveFormatFileEnc(XMLFILE, doc, "UTF-8", 1);
	
	/*free the document */
	xmlFreeDoc(doc);
	
	/*
     	 *Free the global variables that may
     	 *have been allocated by the parser.
     	 */
    	xmlCleanupParser();

    	/*
     	* this is to debug memory for regression tests
     	*/
    	xmlMemoryDump();
    	return 1;
}
#else
char* gets_setting(char* gigo) {
    	#if VERBOSE >= 1
	g_errorf(stderr, "tree support not compiled in\n");
	#endif
    	return NULL
}
static void print_element_names(xmlNode * a_node){}
int new_xml_file() {return 0;}
int xml_file_exists(){return 0;}
#endif //ifdef LIBXML_OUTPUT_ENABLED
#endif //ifdef LIBXML_TREE_ENABLED

#else //ifdev win32

#include <glib.h>
#include <gconf/gconf-client.h>
#include <gio/gio.h>
GSettings *settings;

char* get_setting_str( gchar* keyname) {
	/*
	 * we search based on keyname, and return the value
	 */
	#if VERBOSE >= 3
	printf("Trying to get Setting for Value:%s\n",keyname);
	#endif
	char* valueStr = NULL;
	valueStr = g_settings_get_string (settings,keyname);
	#if VERBOSE >= 4
	printf("Value for key is:%s\n",valueStr);
	#endif
	if (valueStr == NULL) {
       		#if VERBOSE >= 2 	
		g_error("Error: No Value for %s",keyname);
		#endif
		return NULL;
	}
	return valueStr;
}

int get_setting_int( gchar* keyname) {
        /*
         * we search based on keyname, and return the value
         */
	#if VERBOSE >= 3
        printf("Trying to get Setting for Value:%s\n",keyname);
        #endif
        int valueInt = 0;
        valueInt = g_settings_get_int(settings,keyname);
	#if VERBOSE >= 4
        printf("Value for key is:%s\n",valueInt);
        #endif
        if (valueInt == 0) {
                #if VERBOSE >= 2        
                g_error("Error: No Value for %s",keyname);
                #endif
                return -1;
        }
        return valueInt;
}


void settings_unref(){
	if (settings){
		g_object_unref(settings);
	}
}

gboolean settings_init(){
	//This section sould run, but need's error checking
	
	//g_type_init();
	settings = g_settings_new("apps.noside.rchip.settings");
	
	/* catch emitted signal "changed" here*/	
	return TRUE;
}
#endif //ifdef win32
