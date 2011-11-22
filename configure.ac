m4_define([rchip_version_major], [3])
m4_define([rchip_version_minor], [0])
m4_define([rchip_version_micro], [0])

dnl set to 0 when doing an official release
m4_define([rchip_version_pre_release], [0])

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
AC_CONFIG_MACRO_DIR(m4)
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([rchip], rchip_version)

AC_SUBST(ACLOCAL_AMFLAGS, "-I m4")

AC_PROG_LIBTOOL
AC_PROG_INSTALL

AC_DEFINE(COMPILE_YEAR,2011,"Year Compiled")
AC_DEFINE(PROGRAMMERS_NAME,"Kevin Anthony","The Name of the Programmer")

# Checks for programs.
AC_PROG_CC
AC_HEADER_STDC
AC_ISC_POSIX
AC_CHECK_SIZEOF(long)

AC_PATH_PROG(GDK_PIXBUF_CSOURCE, gdk-pixbuf-csource)

# Checks for libraries.


dnl Chack for JSON
RCHIP_CHECK_JSON
dnl Check for GLIB, GTK, GIO, and GSETTINGS
RCHIP_CHECK_GLIB

GLIB_GSETTINGS

dnl check for XML
RCHIP_CHECK_XML

dnl set verbosity level for logging
RCHIP_VERBOSITY

dnl check for music programs
RCHIP_CHECK_MUSIC

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h assert.h sys/utsname.h glob.h curl/curl.h])

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
m4/Makefile
])

