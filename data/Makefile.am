SUBDIRS = icons-hicolor ui pluggable-xml settings-xml

apps.noside.rchip.gschema.xml:
	cp apps.noside.rchip.gschema.xml.in apps.noside.rchip.gschema.xml
gsettings_SCHEMAS = apps.noside.rchip.gschema.xml
@GSETTINGS_RULES@

desktopdir = $(datadir)/applications
desktop_DATA = rchip.desktop

EXTRA_DIST = 			\
	$(desktop_DATA)		\
	$(gsettings_SCHEMAS)

CLEANFILES = apps.noside.rchip.gschema.xml

DISTCLEANFILES = Makefile

MAINTAINERCLEANFILES = \
        Makefile.in
 
