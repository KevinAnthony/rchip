AC_DEFUN([RCHIP_CHECK_MUSIC],
[
	AC_ARG_ENABLE(banshee,AS_HELP_STRING(--disable-banshee,Disable using Banshee Music Program over Rhythmbox),[BANSHEE="no"],[BANSHEE="yes"])
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
])
