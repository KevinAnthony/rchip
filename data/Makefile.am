SUBDIRS = icons-hicolor ui

gsettings_SCHEMAS = apps.noside.rchip.gschema.xml
@GSETTINGS_RULES@

desktopdir = $(datadir)/applications
desktop_DATA = rchip.desktop

EXTRA_DIST = 			\
	$(desktop_DATA)		\
	$(gsettings_SCHEMAS)

CLEANFILES = Makefile

MAINTAINERCLEANFILES = \
        Makefile.in
 
