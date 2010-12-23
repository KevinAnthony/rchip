#include "settings.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <string.h>

//#ifdef LIBXML_TREE_ENABLED
//#ifdef LIBXML_OUTPUT_ENABLED

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
/*#else
char* getsetting(char* gigo) {
    	#ifndef _SILENT
	printf(stderr, "tree support not compiled in\n");
	#endif
    	return NULL
}
static void print_element_names(xmlNode * a_node){}
int new_xml_file() {return 0;}
int xml_file_exists(){return 0;}
#endif
#endif*/
