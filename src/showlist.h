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

#ifndef             SHOWLIST_H
#define             SHOWLIST_H
#include            <gtk/gtk.h>
enum {
    FTDIR,
    FTFILE,
    FTDONOTPROC
};
void                 file_thread_handler          ( gpointer* );
gpointer*            add_file_to_playqueue        ( gpointer* );
gpointer*            add_folder_to_playqueue      ( gpointer* );
int                  file_type                    ( char* );
char*                standard_name                ( char* );
char*                unstandard_name              ( char* );

#endif
