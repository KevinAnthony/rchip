SUBDIRS = icons

gsetings__SCHEMAS = apps.noside.rchip.gschema.xml
@GSETTINGS_RULES@

desktopdir = $(datadir)/applications
desktop_DATA = rchip.desktop

uidir = $(datadir)/noside/ui
ui_DATA = rchip_status_window.glade

XTRA_DIST = 	$(desktop_DATA)
		$(ui_DATA)

CLEANFILES = Makefile 
