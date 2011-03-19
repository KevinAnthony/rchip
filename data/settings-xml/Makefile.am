settings_xml = apps.noside.rchip.gschema.xml

all:
	(for icon_desc in $(settings_xml); do \
		cat $$icon_desc | ${SED} 's,MUSIC.XML,@MUSIC@,' | ${SED} 's,VIDEO.XML,@VIDEO@,'>../$$icon_desc; \
	done;) &>/dev/null || true


DISTCLEANFILES = Makefile ../$(settings_xml)

MAINTAINERCLEANFILES = Makefile.in 

EXTRA_DIST = $(wildcard *.xml)
