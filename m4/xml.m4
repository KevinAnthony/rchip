AC_DEFUN([RCHIP_CHECK_XML],
[
	LIBXML2_REQUIRED=2.5.10
	PKG_CHECK_MODULES(LIBXML2, libxml-2.0 >= $LIBXML2_REQUIRED)
	LIBXML_VERSION=`xml2-config --version`
	AC_SUBST(LIBXML2_CFLAGS)
	AC_SUBST(LIBXML2_LIBS)
])
