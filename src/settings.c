#include <config.h>

#include "settings.h"
#include <string.h>

#ifdef _WIN32
#include <parser.h>
#include <tree.h>

#ifdef LIBXML_TREE_ENABLED
#ifdef LIBXML_OUTPUT_ENABLED

char* getsetting(char* settingname){
	
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
        	printf("error: could not parse file %s\n", XMLFILE);
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
char* getsetting(char* gigo) {
    	#ifndef _SILENT
	printf(stderr, "tree support not compiled in\n");
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
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void keyChangeCallback(GConfClient* client,guint cnxn_id,GConfEntry* entry,gpointer userData) {
	const GConfValue* value = NULL;
  	const gchar* keyname = NULL;
  	gchar* strValue = NULL;
	#ifdef _DEBUG
  		printf("keyChangeCallback invoked.\n");
	#endif
 	keyname = gconf_entry_get_key(entry);
	if (keyname == NULL) {
    		#ifndef _SILENT
			printf("Couldn't get the key name!\n");
		#endif
		return;
  	}
	value = gconf_entry_get_value(entry);
  	g_assert(value != NULL);
	if (!GCONF_VALUE_TYPE_VALID(value->type)) {
    		#ifndef _SILENT
			printf("Invalid type for gconfvalue!\n");
		#endif
  	}
	strValue = gconf_value_to_string(value);
	#ifdef _DEBUG
		if (strcmp(keyname, SERVICE_KEY_PATH_TO_VIDEO_ROOT) == 0) {
			printf("Connection type setting changed: [%s]\n",strValue);
  		} else {
  	  		printf(":Unknown key: %s (value: [%s])\n", keyname,strValue);
  		}
	#endif
  	g_free(strValue);
	#ifdef _DEBUG
  		printf("keyChangeCallback done.\n");
	#endif
}

int registerCallback( void (*regesterFunction) (void),gchar* registerKey){
	/*
	 * Here we would register a function to a list or something
	 * I'm unsure if how to prototype this
	 * The secound value is which key the function should respond to
	 */
	return true;
}

int unregisterCallback( void (*regesterFunction) (gchar*)){
	/*
	 * here we would unregister the function, just like above
	 */
	return true;
}

int callbackRegisteredFunctions(gchar* keyname){
	/*
	 * we need to search the reigsteredFunctions by keyname, 
	 * and callback the function based on which key it called
	 */
	return true;
}

char* getsetting( gchar* keyname) {
	/*
	 * we search based on keyname, and return the value
         */
	GConfClient* client = NULL;
	client = gconf_client_get_default();
        g_assert(GCONF_IS_CLIENT(client));
	gchar* valueStr = NULL;
	gchar* lookup = (gchar*) malloc(sizeof(keyname) + sizeof(SERVICE_GCONF_ROOT)+5);
	sprintf(lookup,SERVICE_GCONF_ROOT "%s", keyname);
        valueStr = gconf_client_get_string(client,lookup, NULL);
	g_free(lookup);
        if (valueStr == NULL) {
        	//#ifndef _SILENT
		printf("Error: No Value for %s",keyname);
		//#endif
		return NULL;
	}
	return valueStr;
}

void populate_defaults(GConfClient* client){
 	gchar* valueStr = NULL;
  	valueStr = gconf_client_get_string(client, SERVICE_KEY_PATH_TO_VIDEO_ROOT, NULL);
  	if (valueStr == NULL) {	
		if (!gconf_client_set_string(client, SERVICE_KEY_PATH_TO_VIDEO_ROOT, "/media/Tamatebako/",NULL)) {
		}
	}
}
int settings_init(){
	//This section sould run, but need's error checking
	
	GConfClient* client = NULL;
	GError* error = NULL;
	g_type_init();
	client = gconf_client_get_default();
	g_assert(GCONF_IS_CLIENT(client));
	gconf_client_add_dir(client, SERVICE_GCONF_ROOT, GCONF_CLIENT_PRELOAD_NONE, &error);
	gconf_client_notify_add(client, SERVICE_GCONF_ROOT,keyChangeCallback, NULL, NULL, &error);
	populate_defaults(client);
	return true;
}
#endif //ifdef win32
