/*
 *
 *    rchip, Remote Controlled Home Integration Program
 *    Copyright (C) 2011-2012 <kevin.s.anthony@gmail.com
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef STATUS_H
#define STATUS_H
#include            <gtk/gtk.h>

void                gui_thread_handler              ( gpointer* );
void                init_status_window              ( gboolean );
void                init_status_labels              ( GtkBuilder* );
void                init_info_labels                ( GtkBuilder* );
void                init_xml_labels                 ( GtkBuilder* );
void                init_buttons                    ( GtkBuilder* );
void                window_destroyed                ( GtkWidget*,gpointer );
void                close_window                    ( GtkWidget*,gpointer );
void                show_hide_window                ( void );
char*               get_file_name_from_path         ( char* );
gpointer*           insert_into_window              ( gpointer* );
gpointer*           read_to_buffer                  ( gpointer* );
gpointer*           write_to_buffer                 ( gpointer* );
#endif
