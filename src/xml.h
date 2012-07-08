/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
*
*    rchip, Remote Controlled Home Integration Program
*    Copyright (C) 2011 <Kevin@NoSideRacing.com
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
#ifndef             XML_H
#define             XML_H

#include            <libxml/parser.h>
#include            <libxml/tree.h>
#include            <glib.h>

#ifdef              LIBXML_TREE_ENABLED
#ifdef              LIBXML_OUTPUT_ENABLED
#define             VALID_XML

gboolean            xml_init                    ( void );
void                xml_free                    ( void );
void                xml_settings_callback       ( void );
gboolean            xml_find_command            ( char* );
void                find_command                ( xmlNode*,char* );
gboolean            xml_file_exists             ( void );
char*               xml_get_type                ( void );
char*               xml_get_music_or_video      ( void );
gboolean            xml_has_system_argument     ( void );
char*               xml_get_dbus_argument       ( void );
char*               xml_get_dbus_argument_type  ( void );
char*               xml_get_dbus_command        ( void );
char*               xml_get_system_command      ( void );
char*               xml_get_bus_name            ( char* );
char*               xml_get_bus_path            ( char* );

#endif
#endif

#endif
