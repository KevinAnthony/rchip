SUBDIRS = icons

gsetings__SCHEMAS = apps.noside.rchip.gschema.xml
@GSETTINGS_RULES@

desktopdir = $(datadir)/applications
desktop_DATA = rchip.desktop

#if GSETTING_SCHEMAS_INSTALL
#install-data-local: $(schema_DATA)
#        GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) $(GCONFTOOL) --makefile-install-rule $(srcdir)/$(schema_DATA);
#endif


XTRA_DIST = 	$(desktop_DATA)


CLEANFILES = Makefile 
