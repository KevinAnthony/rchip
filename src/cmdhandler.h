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
#ifndef             CMDHANDLER_H
#define             CMDHANDLER_H

#include            <glib.h>
#include            "utils.h"

gboolean            delete_from_hosts                ( char* );
char*               get_next_host                    ( void*,size_t,size_t );
void                print_list                       ( void );
void                get_next_cmd                     ( void );
gboolean            process_cmd                      ( char*,char* );
void                send_cmd                         ( char*,char*,thread_priority );

#endif
