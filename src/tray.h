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
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef                TRAY_H
#define                TRAY_H
#include            <gtk/gtk.h>

void                 start_tray                      ( );
#ifdef GTK3
void                 activate                        ( GtkApplication* );
#endif
void                 tray_click                      ( GtkStatusIcon*,gpointer );
void                 tray_menu                       ( GtkStatusIcon*, guint, guint, gpointer );
void                 add_files                       ( GtkWidget*, gpointer );
void                 add_folders                     ( GtkWidget*, gpointer );
void                 about_box                       ( GtkWidget*, gpointer );
void                 change_music                    ( GtkWidget*, gpointer );
void                 change_video                    ( GtkWidget*, gpointer );
GtkStatusIcon*       create_tray_icon                ( void );
GtkWidget*           create_tray_menu                ( GtkStatusIcon* );
void                 set_xml_menu                    ( GtkWidget*,GtkWidget*);
void                 set_xml_menu_with_path          ( GtkWidget*,GtkWidget*,char*);
#endif
