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

#ifndef SQL_H
#define SQL_H

#ifdef _SQL

#ifdef _WIN32
	#include        <winsock.h>
	#include        <mysql.h>
#else
	#include	<mysql/mysql.h>
#endif
#include	<glib.h>

gboolean sql_init();
int sql_is_connected(int);
void sql_exec_quary(char*);
void get_next_cmd_from_sql(char*,int*,char**,char**,char**);
void delete_from_cmdQueue(int);
char* get_registered_devices_message();
void get_active_devices(void*, size_t, size_t);
size_t get_size();
size_t get_nelem();
void update_daemon_sql();
#endif //#ifdef _SQL
#endif
