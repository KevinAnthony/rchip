AUTOMAKE_OPTIONS = foreign

CFLAGS=-std=c99 -Wall -g 
CPPFLAGS = -DPREFIX='"$(datadir)"' @CPPFLAGS@

AM_CFLAGS = $(CFLAGS) @GTK_CFLAGS@ @GLIB_CFLAGS@ @LIBXML2_CFLAGS@ @DBUS_CFLAGS@ @GCONF_CFLAGS@ @MYSQL_CFLAGS@
sourceincludedir = $(includedir)/msdaemon/source
sourceinclude_HEADERS = 		\
	cmdhandler.h			\
	dbus.h  			\
	mstray.h			\
	notifications.h			\
	settings.h  			\
	showlist.h  			\
	sql.h  				\
	utils.h				
sourceinclude_SOURCE =			\
	$(sourceinclude_HEADERS) 	\
	cmdhandler.c                    \
        dbus.c                          \
        mstray.c                        \
        notifications.c                 \
        settings.c                      \
        showlist.c                      \
        sql.c                           \
        utils.c
 
bin_PROGRAMS = msdaemon
msdaemon_SOURCES = $(sourceinclude_SOURCE) msdaemon.c
msdaemon_LDADD = @GTK_LIBS@ @GLIB_LIBS@ @DBUS_LIBS@ @LIBXML2_LIBS@ @GCONF_LIBS@ @MYSQL_LIBS@

CLEANFILES = Makefile .deps/*

clean-local:
	-rmdir .deps
