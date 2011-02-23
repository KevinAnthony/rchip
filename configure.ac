#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.67])
AC_INIT([msDaemon], [0.1.0.1], [kevin@nosideracing.com])
AC_CONFIG_SRCDIR([src/sql.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([msDaemon], [0.1.0.1])

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
PKG_CHECK_MODULES(GTK, gtk+-2.0 >=  2.10.0, HAVE_GTK="yes", sweep_config_ok="no")
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)
if test "x$HAVE_GTK" != xyes ; then
    AC_CHECK_LIB(gtk-x11-2.0, gtk_init, HAVE_GTK="maybe", sweep_config_ok="no")
fi

PKG_CHECK_MODULES(GLIB, glib-2.0 >=  2.2.0, HAVE_GLIB="yes", sweep_config_ok="no")
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)
if test "x$HAVE_GLIB" != xyes ; then
  AC_CHECK_LIB(glib-2.0, g_list_append, HAVE_GLIB="maybe", sweep_config_ok="no")
fi

#PKG_CHECK_MODULES(GTHREADS, gthread-2.0 >=  2.2.0, HAVE_GTHREADS="yes", sweep_config_ok="no")
#AC_SUBST(GTHREADS_CFLAGS)
#AC_SUBST(GTHREADS_LIBS)
#if test "x$HAVE_GTHREADS" != xyes ; then
#  AC_CHECK_LIB(gthread-2.0, g_thread_init, HAVE_GTHREADS="maybe", sweep_config_ok="no")
#fi

PKG_CHECK_MODULES(DBUS, dbus-glib-1 >= 0.70 dbus-1 >= 0.70)
AC_SUBST(DBUS_CFLAGS)
AC_SUBST(DBUS_LIBS)
if test "x$HAVE_DBUS" != xyes ; then
  AC_CHECK_LIB(dbug-1, dbus_init, HAVE_DBUS="maybe", sweep_config_ok="no")
fi

LIBXML2_REQUIRED=2.5.10
PKG_CHECK_MODULES(LIBXML2, libxml-2.0 >= $LIBXML2_REQUIRED)
LIBXML_VERSION=`xml2-config --version`
AC_SUBST(LIBXML2_CFLAGS)
AC_SUBST(LIBXML2_LIBS)

PKG_CHECK_MODULES(GCONF, [gconf-2.0])
AC_SUBST(GCONF_CFLAGS)
AC_SUBST(GCONF_LIBS)
if test "x$HAVE_GCONF" != xyes ; then
	AC_CHECK_LIB(gconf-2.0, gconf_client_get_default, HAVE_GCONF="maybe", sweep_config_ok="no")
fi

AC_ARG_ENABLE(mysql,[  --disable-mysql         Turns off SQL(program does not do anything],[MYSQL="no"],[MYSQL="yes"])
MYSQLCONFIG="/usr/bin/mysql_config"
if test "x$MYSQL" = "xno"; then
	AC_DEFINE(_NOSQL,,"Turn OFF SQL Functionality")
else
	MYSQL_CFLAGS=`$MYSQLCONFIG --cflags`
	MYSQL_INCLUDES=`$MYSQLCONFIG --include`
	MYSQL_LIBS=`$MYSQLCONFIG --libs_r`
	AC_DEFINE(_SQL,,"Turn ON SQL Functionality")
fi
AC_SUBST(MYSQL_CFLAGS)
AC_SUBST(MYSQL_INCLUDES)
AC_SUBST(MYSQL_LIBS)

AC_MSG_CHECKING(for MySQL CFLAGS)
AC_MSG_RESULT($MYSQL_CFLAGS)
AC_MSG_CHECKING(for MySQL includes)
AC_MSG_RESULT($MYSQL_INCLUDES)
AC_MSG_CHECKING(for MySQL libraries)
AC_MSG_RESULT($MYSQL_LIBS)

AC_ARG_ENABLE(debugging,[  --enable-debugging      print verbose output for debuging],AC_DEFINE(_DEBUG,,"Verbose Outputting"),AC_DEFINE(_SILENT,,"Quiet Outpitting"))

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([strstr uname])

AC_CONFIG_FILES([Makefile src/Makefile data/Makefile])
AC_OUTPUT
