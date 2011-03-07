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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <glib/gprintf.h>
#include "utils.h"

char* replace_str(char* str, char* orig, char* rep){
	static char buffer[4096];
	char* p;
	if(!(p = strstr(str, orig)))  // Is 'orig' even in 'str'?
    		return str;
  	strncpy(buffer, str, p-str); // Copy characters from 'str' start to 'orig' st$
  	buffer[p-str] = '\0';
  	g_sprintf(buffer+(p-str), "%s%s", rep, p+strlen(orig));
  	return buffer;
}
