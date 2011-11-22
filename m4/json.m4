AC_DEFUN([RCHIP_CHECK_JSON],
[
	PKG_CHECK_MODULES(LIBCURL, libcurl >= 7.21.3)
	PKG_CHECK_MODULES(LIBJSON, json >= 0.9)
	PKG_CHECK_MODULES(LIBJSONGLIB, json-glib-1.0 >= 0.14.0)
	AC_ARG_ENABLE(json,AS_HELP_STRING(--disable-json,Disable using JSON to interact with database over MYSQL),[JSON="no"],[json="yes"])
	if test "x$json" = "xyes"; then
        	AC_DEFINE(_JSON,,"Enable Banshee")
			AC_SUBST(LIBCURL_CFLAGS)
			AC_SUBST(LIBCURL_LIBS)
			AC_SUBST(LIBJSON_CFLAGS)
			AC_SUBST(LIBJSON_LIBS)
			AC_SUBST(LIBJSONGLIB_CFLAGS)
			AC_SUBST(LIBJSONGLIB_LIBS)
        fi
])

