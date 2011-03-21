m4_define([rchip_version_major], [2])
m4_define([rchip_version_minor], [0])
m4_define([rchip_version_micro], [0])

dnl set to 0 when doing an official release
m4_define([rchip_version_pre_release], [13])

m4_define([concat], $1$2$3$4)

dnl create base version string
m4_define([rchip_version], concat(
        rchip_version_major.,
        rchip_version_minor.,
        rchip_version_micro))

dnl create the release version
m4_define([rchip_version],
        m4_if(rchip_version_pre_release, [0],
                rchip_version,
                concat(rchip_version-, rchip_version_pre_release)))

m4_define([rchip_api_version],
        [rchip_version_major.rchip_version_minor])

m4_define([rchip_asm_version],
        [rchip_api_version.0.0])



AC_PREREQ([2.67])
AC_INIT([rchip], rchip_version, [kevin@nosideracing.com])
AC_CONFIG_SRCDIR([src/sql.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([rchip], rchip_version)
#for internatioanl suppoert
#IT_PROG_INTLTOOL([0.35.0])
AC_PROG_LIBTOOL
AC_PROG_INSTALL

# Checks for programs.
AC_PROG_CC
AC_HEADER_STDC
AC_ISC_POSIX
AC_CHECK_SIZEOF(long)

AC_PATH_PROG(GDK_PIXBUF_CSOURCE, gdk-pixbuf-csource)

# Checks for libraries.

PKG_CHECK_MODULES([GSETTINGS],[gio-2.0 >= 2.25.0],[HAVE_GSETTINGS=yes],[HAVE_GSETTINGS=no])
if test "x$HAVE_GSETTINGS" = "xyes"; then
	AC_DEFINE([HAVE_GSETTINGS], 1, [Whether we can use GSettings])
	GLIB_GSETTINGS
fi

PKG_CHECK_MODULES(GTK3, gtk+-3.0 >=  3.00.0, HAVE_GTK3="yes", HAVE_GTK3="no")
if test "x$HAVE_GTK3" = "xyes" ; then
    	AC_MSG_NOTICE([USING GTK3])
	AC_SUBST(GTK3_CFLAGS)
    	AC_SUBST(GTK3_LIBS)
    	AC_CHECK_LIB(gtk-x11-3.0, gtk_init, HAVE_GTK="maybe", sweep_config_ok="no")
	AC_DEFINE(GTK3,,"We are using GTK 3")
else
	PKG_CHECK_MODULES(GTK2, gtk+-2.0 >=  2.10.0, HAVE_GTK="yes", sweep_config_ok="no")
	if test "x$HAVE_GTK" = "xyes" ; then
		AC_DEFINE(GTK2,,"We are using GTK 2")
		AC_MSG_NOTICE([USING GTK2])
		AC_SUBST(GTK2_CFLAGS)
		AC_SUBST(GTK2_LIBS)
		if test "x$HAVE_GTK" != xyes ; then
		    AC_CHECK_LIB(gtk-x11-2.0, gtk_init, HAVE_GTK="maybe", sweep_config_ok="no")
		fi
	else
		AC_MSG_ERROR([NO GTK found, please install GTK])
	fi	
fi
PKG_CHECK_MODULES(GLIB, glib-2.0 >=  2.28.1, HAVE_GLIB="yes", sweep_config_ok="no")
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)
if test "x$HAVE_GLIB" != "xyes" ; then
  AC_CHECK_LIB(glib-2.0, g_list_append, HAVE_GLIB="maybe", sweep_config_ok="no")
fi

#PKG_CHECK_MODULES(DBUS, dbus-glib-1 >= 0.70 dbus-1 >= 0.70)
#AC_SUBST(DBUS_CFLAGS)
#AC_SUBST(DBUS_LIBS)
#if test "x$HAVE_DBUS" != "xyes" ; then
#  AC_CHECK_LIB(dbug-1, dbus_init, HAVE_DBUS="maybe", sweep_config_ok="no")
#fi

LIBXML2_REQUIRED=2.5.10
PKG_CHECK_MODULES(LIBXML2, libxml-2.0 >= $LIBXML2_REQUIRED)
LIBXML_VERSION=`xml2-config --version`
AC_SUBST(LIBXML2_CFLAGS)
AC_SUBST(LIBXML2_LIBS)

#PKG_CHECK_MODULES(GCONF, [gconf-2.0])
#AC_SUBST(GCONF_CFLAGS)
#AC_SUBST(GCONF_LIBS)
#if test "x$HAVE_GCONF" != "xyes" ; then
#	AC_CHECK_LIB(gconf-2.0, gconf_client_get_default, HAVE_GCONF="maybe", sweep_config_ok="no")
#fi

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

AC_ARG_ENABLE(debug,AS_HELP_STRING(--enable-debug={0-5},prints verbose output for debugging))
case "x$enable_debug" in
	x)	AC_DEFINE(VERBOSE,1,"Output Verbosity Level");;
	x0)	AC_DEFINE(VERBOSE,0,"Output Verbosity Level");;
	x1)	AC_DEFINE(VERBOSE,1,"Output Verbosity Level");;
	x2)	AC_DEFINE(VERBOSE,2,"Output Verbosity Level");;
	x3)	AC_DEFINE(VERBOSE,3,"Output Verbosity Level");;
	x4)	AC_DEFINE(VERBOSE,4,"Output Verbosity Level");;
	x5)	AC_DEFINE(VERBOSE,5,"Output Verbosity Level");;
	*)	AC_MSG_ERROR([unexpected value $enable_debug for --enable-debug configure option]);;
esac

AC_DEFINE(COMPILE_YEAR,2011,"Year Compiled")
AC_DEFINE(PROGRAMMERS_NAME,"Kevin Anthony","The Name of the Programmer")

#Check for Music Programs
AC_ARG_ENABLE(banshee,AS_HELP_STRING(--enable-banshee,Enables using Banshee Music Program over Rhythmbox),[BANSHEE="yes"],[BANSHEE="no"])
AC_ARG_ENABLE(dbus,AS_HELP_STRING(--disable-dbus,disables dbus interface preventing music remote from working),[DBUS="no"],[DBUS="yes"])

AC_CHECK_PROG(HAVE_RHYTHMBOX, rhythmbox, true, false)
AC_CHECK_PROG(HAVE_BANSHEE, banshee, true, false)

if test "x$DBUS" != "xno"; then
        if test "x$HAVE_RHYTHMBOX" = "xfalse" -a "x$HAVE_BANSHEE" = "xfalse" -a "x$DBUS" = "xyes"; then
                AC_MSG_ERROR([*** dbus enabled but no music player found, please install either RHYTHMBOX or BANSHEE or use --disable-dbus])
        else
                if test "x$BANSHEE" = "xyes"; then
			if test "x$HAVE_BANSHEE" = "xfalse"; then
				AC_MSG_ERROR([*** --enable-banshee flag used, but BANSHEE not present, please install BANSHEE or do not use flag])
			else
				AC_DEFINE(BANSHEE,,"Enable Banshee")
				AC_SUBST(MUSIC,"${datadir}/noside/xml/banshee.music.xml")
			fi
		else
			if test "x$HAVE_RHYTHMBOX" = "xfalse" ; then
				AC_DEFINE(BANSHEE,,"Enable Banshee")
			else
              			AC_DEFINE(RHYTHMBOX,,"Enable Rhythmbox")
				AC_SUBST(MUSIC,"${datadir}/noside/xml/rhythmbox.music.xml")
			fi
		fi
        fi
else
	AC_DEFINE(DBUSOFF,,"Disable Dbus totally")
fi
AC_SUBST(VIDEO,"${datadir}/noside/xml/smplayer.video.xml")

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h assert.h sys/utsname.h glob.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_HEADER_STDBOOL

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS(strlen)
AC_CHECK_FUNCS(uname)
AC_CHECK_FUNCS(atoi)
AC_CHECK_FUNCS(strcat)
AC_CHECK_FUNCS(strncpy)
AC_CHECK_FUNCS(glob)
AC_CHECK_FUNCS(globfree)

AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(uint)
AC_CHECK_SIZEOF(double)
AC_CHECK_SIZEOF(size_t)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(ulong)
AC_CHECK_SIZEOF(glob_t)
AC_OUTPUT([
Makefile
src/Makefile
data/Makefile
data/icons-hicolor/Makefile
data/ui/Makefile
data/pluggable-xml/Makefile
data/settings-xml/Makefile
man/Makefile
doc/Makefile
])

