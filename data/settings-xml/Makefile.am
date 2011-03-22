settings_xml = apps.noside.rchip.gschema.xml

all:
	(for xml_desc in $(settings_xml); do \
		cat $$xml_desc | ${SED} 's,MUSIC.XML,@MUSIC@,' | ${SED} 's,VIDEO.XML,@VIDEO@,'>../$$xml_desc; \
	done;) &>/dev/null || true

DISTCLEANFILES = Makefile ../$(settings_xml)

MAINTAINERCLEANFILES = Makefile.in 

EXTRA_DIST = $(wildcard *.xml)
