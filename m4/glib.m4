AC_DEFUN([RCHIP_CHECK_GLIB],
[
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

	PKG_CHECK_MODULES(GTHREAD, gthread-2.0 >=  2.32.3, HAVE_GTHREAD="yes", sweep_config_ok="no")
	AC_SUBST(GTHREAD_CFLAGS)
	AC_SUBST(GTHREAD_LIBS)
	if test "x$HAVE_GTHREAD" != "xyes" ; then
		AC_CHECK_LIB(gthread-2.0, g_thread_init, HAVE_GTHREAD="maybe", sweep_config_ok="no")
	fi
])

