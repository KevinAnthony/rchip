AUTOMAKE_OPTIONS = foreign

CFLAGS=-std=c99 -Wall -g 
CPPFLAGS = -DPREFIX='"$(datadir)"' @CPPFLAGS@

AM_CFLAGS = $(CFLAGS) @GTK2_CFLAGS@ @GTK3_CFLAGS@ @GLIB_CFLAGS@ @LIBXML2_CFLAGS@ @DBUS_CFLAGS@ @GCONF_CFLAGS@ @MYSQL_CFLAGS@
sourceincludedir = $(includedir)/rchip/source
sourceinclude_HEADERS = 		\
	cmdhandler.h			\
	dbus.h  			\
	tray.h			\
	notifications.h			\
	settings.h  			\
	showlist.h  			\
	sql.h  				\
	utils.h				
sourceinclude_SOURCE =			\
	$(sourceinclude_HEADERS) 	\
	cmdhandler.c                    \
        dbus.c                          \
        tray.c                        \
        notifications.c                 \
        settings.c                      \
        showlist.c                      \
        sql.c                           \
        utils.c
 
bin_PROGRAMS = rchip
rchip_SOURCES = $(sourceinclude_SOURCE) main.c
rchip_LDADD = @GTK2_LIBS@ @GTK3_LIBS@ @GLIB_LIBS@ @DBUS_LIBS@ @LIBXML2_LIBS@ @GCONF_LIBS@ @MYSQL_LIBS@

CLEANFILES = Makefile .deps/*

clean-local:
	-rmdir .deps
