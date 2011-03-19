AUTOMAKE_OPTIONS = foreign

CFLAGS=-std=c99 -Wall -g 
CPPFLAGS = -DPREFIX='"$(datadir)"' @CPPFLAGS@

AM_CFLAGS = 	$(CFLAGS) 		\
		@GTK2_CFLAGS@ 		\
		@GTK3_CFLAGS@		\
		@GLIB_CFLAGS@		\
		@LIBXML2_CFLAGS@	\
		@MYSQL_CFLAGS@

sourceincludedir = $(includedir)/@PACKAGE@/source
sourceinclude_HEADERS = 		\
	cmdhandler.h			\
	dbus.h  			\
	tray.h				\
	status.h			\
	notifications.h			\
	settings.h			\
	showlist.h			\
	sql.h				\
	xml.h				\
	utils.h

sourceinclude_SOURCE =			\
	$(sourceinclude_HEADERS)	\
	cmdhandler.c			\
	dbus.c				\
	tray.c				\
	status.c			\
	notifications.c			\
	settings.c			\
	showlist.c			\
	sql.c				\
	xml.c				\
	utils.c

bin_PROGRAMS = rchip 

rchip_SOURCES = $(sourceinclude_SOURCE) main.c
rchip_LDADD = 	@GTK2_LIBS@		\
		@GTK3_LIBS@		\
		@GLIB_LIBS@		\
		@LIBXML2_LIBS@		\
		@MYSQL_LIBS@

DISTCLEANFILES = Makefile

MAINTAINERCLEANFILES = \
	Makefile.in

distclean-local:
	rm -rf .deps
