AUTOMAKE_OPTIONS = foreign

CFLAGS=-std=gnu99 -Wall -ggdb
CPPFLAGS = -DPREFIX='"$(datadir)"' @CPPFLAGS@

AM_CFLAGS = 	$(CFLAGS)   \
		@GTK2_CFLAGS@ 		\
		@GTK3_CFLAGS@		\
		@GTHREAD_CFLAGS@	\
		@GLIB_CFLAGS@		\
		@LIBXML2_CFLAGS@	\
		@LIBJSON_CFLAGS@	\
		@LIBCURL_CFLAGS@

sourceincludedir = $(includedir)/@PACKAGE@/source
sourceinclude_HEADERS = \
	cmdhandler.h		\
	dbus.h  			\
	tray.h				\
	status.h			\
	notifications.h		\
	settings.h			\
	showlist.h			\
	xml.h				\
	rest.h				\
	utils.h

sourceinclude_SOURCE =	\
	$(sourceinclude_HEADERS)	\
	cmdhandler.c		\
	dbus.c				\
	tray.c				\
	status.c			\
	notifications.c		\
	settings.c			\
	showlist.c			\
	xml.c				\
	rest.c				\
	utils.c

bin_PROGRAMS = rchip

rchip_SOURCES = $(sourceinclude_SOURCE) main.c
rchip_LDADD = 	\
		@GTK2_LIBS@		\
		@GTK3_LIBS@		\
		@GTHREAD_LIBS@	\
		@GLIB_LIBS@		\
		@LIBXML2_LIBS@	\
		@LIBJSON_LIBS@	\
		@LIBCURL_LIBS@

DISTCLEANFILES = Makefile

MAINTAINERCLEANFILES = \
	Makefile.in

distclean-local:
	rm -rf .deps
