#include <assert.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <libxml/tree.h>
#include <libxml/parser.h>

#include "xml.h"
#include "settings.h"

#ifdef VALID_XML

xmlNode *cmd_node = NULL;
xmlNode *root_element_music = NULL;
xmlNode *root_element_video = NULL;
xmlDoc *doc_music = NULL;
xmlDoc *doc_video = NULL;
char* MUSIC_OR_VIDEO = NULL;

gboolean xml_init(){
	settings_init();
	gboolean retVal = TRUE;
	if (!(xml_file_exists())){
		#if VERBOSE >= 1
		g_error("xml_file_exists RETURNED FALSE\n");
		#endif
		return FALSE;
       	} 

	LIBXML_TEST_VERSION

	/*parse the file and get the DOM */
	char* musicxml = g_strdup(get_setting_str(MUSIC_XML));
	char* videoxml = g_strdup(get_setting_str(VIDEO_XML));
	doc_music = xmlReadFile(musicxml, NULL, 0);
	doc_video = xmlReadFile(videoxml, NULL, 0);

	if (doc_music == NULL) {
		#if VERBOSE >= 1
		g_error("error: could not parse file %s\n", musicxml);
		#endif
		retVal = FALSE;
	}
	if (doc_video == NULL) {
		#if VERBOSE >= 1
		g_error("error: could not parse file %s\n",videoxml);
		#endif
		retVal = FALSE;
	}	

	/*Get the root element node */
	root_element_music = xmlDocGetRootElement(doc_music);
	root_element_video = xmlDocGetRootElement(doc_video);
	return retVal;
}

void xml_free(){
	xmlFreeDoc(doc_music);
	xmlFreeDoc(doc_video);
	xmlCleanupParser();
}

void xml_settings_callback(){
	xml_free();
	if (!xml_init()){
		#if VERBOSE >= 1
                g_error("xml_init Failed after setting changed\n");
                #endif
	}
}
gboolean xml_find_command(char* command){
	/*Itterate threw the xmlNodes until we find the command, if it's there, we set cmd_node to the correct node*/
	cmd_node = NULL;
	MUSIC_OR_VIDEO = NULL;
	find_command(root_element_music,command);
	if (cmd_node == NULL){
		find_command(root_element_video,command);
	}
	if (cmd_node == NULL){
		return FALSE;
	}
	return TRUE;
}

void find_command(xmlNode *a_node, char* command){
	xmlNode *cur_node = NULL;
	cur_node = a_node->xmlChildrenNode;
	while (cur_node != NULL) {
		if (!(g_strcmp0((char*)cur_node->name,"command"))){
			xmlNode *child_node = cur_node->xmlChildrenNode;
			while (child_node != NULL){
				if (!(g_strcmp0((char*)child_node->name,"name"))){
					if (!(g_strcmp0((char*)xmlNodeGetContent(child_node->xmlChildrenNode),command))){
						cmd_node = cur_node;
						MUSIC_OR_VIDEO = (char*)a_node->name;
						return;
					}
				}
				child_node = child_node->next;
			}
		}
		cur_node = cur_node->next;
	}
}


gboolean xml_file_exists(){
	char* musicxml = g_strdup(get_setting_str(MUSIC_XML));
	char* videoxml = g_strdup(get_setting_str(VIDEO_XML));
	FILE* file = fopen(musicxml, "r");
	if (file != NULL){
		fclose(file);
		file = fopen(videoxml, "r");
		if (file != NULL){
			fclose(file);
			return TRUE;
		}
	}
	return FALSE;
}

char* xml_get_type(){
	assert(cmd_node != NULL);
	xmlNode *child_node = cmd_node->xmlChildrenNode;
	while (child_node != NULL){
		if (!(g_strcmp0((char*)child_node->name,"type"))){
                	return (char*)xmlNodeGetContent(child_node->xmlChildrenNode);
                }
                child_node = child_node->next;
        }
	return NULL;	
}

char* xml_get_music_or_video(){
	return MUSIC_OR_VIDEO;
}

gboolean xml_has_system_argument(){
	assert(cmd_node != NULL);
	gboolean retVal = FALSE;
	xmlNode *child_node = cmd_node->xmlChildrenNode;
        while (child_node != NULL){
                if (!(g_strcmp0((char*)child_node->name,"argument"))){
                        if (g_strcmp0((char*)xmlNodeGetContent(child_node->xmlChildrenNode),"TRUE")){
				retVal = TRUE;
			}
                }
                child_node = child_node->next;
        }
	return retVal;
}

char* xml_get_dbus_argument(){
	assert(cmd_node != NULL);
	char* retVal = NULL;
        xmlNode *child_node = cmd_node->xmlChildrenNode;
        while (child_node != NULL){
                if (!(g_strcmp0((char*)child_node->name,"argument"))){
                        retVal= (char*)xmlNodeGetContent(child_node->xmlChildrenNode);
			#if VERBOSE >= 4
			g_printf("found Argument\n");
			#endif
                }
                child_node = child_node->next;
        }
        return retVal;
}       

char* xml_get_dbus_argument_type(){
	assert(cmd_node != NULL);
	char* retVal = NULL;
        xmlNode *child_node = cmd_node->xmlChildrenNode;
        while (child_node != NULL){
                if (!(g_strcmp0((char*)child_node->name,"argument_type"))){
                        retVal = (char*)xmlNodeGetContent(child_node->xmlChildrenNode);
                }
                child_node = child_node->next;
        }
        return retVal;
}       

char* xml_get_dbus_command(){
	assert(cmd_node != NULL);
        char* retVal = NULL;
	xmlNode *child_node = cmd_node->xmlChildrenNode;
        while (child_node != NULL){
                if (!(g_strcmp0((char*)child_node->name,"dbus_command"))){
                        retVal =  (char*)xmlNodeGetContent(child_node->xmlChildrenNode);
                }
                child_node = child_node->next;
        }
        return retVal;
}       

char* xml_get_system_command(){
	assert(cmd_node != NULL);
	char* retVal = NULL;
        xmlNode *child_node = cmd_node->xmlChildrenNode;
        while (child_node != NULL){
                if (!(g_strcmp0((char*)child_node->name,"system_command"))){
                        retVal =  (char*)xmlNodeGetContent(child_node->xmlChildrenNode);
                }
                child_node = child_node->next;
        }
        return retVal;
}

char* xml_get_bus_name( char* type ){
	xmlNode *top_node = NULL;
	xmlNode *conn_node = NULL;
	char* retVal = NULL;
	if (!(g_strcmp0(type,"MUSIC"))){
		top_node = root_element_music->xmlChildrenNode;
	} else if (!(g_strcmp0(type,"VIDEO"))){
		top_node = root_element_video->xmlChildrenNode;
	} else {
		return NULL;
	}
	assert(top_node != NULL);
        while (top_node != NULL){
		if (!(g_strcmp0((char*)top_node->name,"connection"))){
			conn_node = top_node->xmlChildrenNode;
			break;
		}
		top_node = top_node->next;
	}
	if (conn_node == NULL){
		#if VERBOSE >= 1
		g_error("No Connection node in XML for %s\n",type);
		#endif
		return NULL;
	}
	xmlNode *child_node = conn_node;
	while (child_node != NULL){
                if (!(g_strcmp0((char*)child_node->name,"bus_name"))){
                        retVal= (char*)xmlNodeGetContent(child_node->xmlChildrenNode);
			break;
                }
                child_node = child_node->next;
        }
        return retVal;
}       

char* xml_get_bus_path ( char* type ){
xmlNode *top_node = NULL;
        xmlNode *conn_node = NULL;
        char* retVal = NULL;
        if (!(g_strcmp0(type,"MUSIC"))){
                top_node = root_element_music->xmlChildrenNode;
        } else if (!(g_strcmp0(type,"VIDEO"))){
                top_node = root_element_video->xmlChildrenNode;
        } else {
                return NULL;
        }
        assert(top_node != NULL);
        while (top_node != NULL){
                if (!(g_strcmp0((char*)top_node->name,"connection"))){
                        conn_node = top_node->xmlChildrenNode;
			break;
                }
                top_node = top_node->next;
        }
        if (conn_node == NULL){
		#if VERBOSE >= 1
                g_error("No Connection node in XML for %s\n",type);
		#endif
                return NULL;
        }
        xmlNode *child_node = conn_node;
        while (child_node != NULL){
                if (!(g_strcmp0((char*)child_node->name,"bus_path"))){
                        retVal= (char*)xmlNodeGetContent(child_node->xmlChildrenNode);
			break;
                }
                child_node = child_node->next;
        }
        return retVal;

}       

#endif
