desktopdir = $(datadir)/applications
desktop_DATA = rchip.desktop

pngdir=$(datadir)/madsci/

png_DATA = 	beaker_blue.png         \
		beaker_red.png		\
		flask_blue.png   	\
		flask_red.png     	\
		beaker_green.png  	\
		beaker_yellow.png  	\
		flask_green.png  	\
		flask_yellow.png  	

themedir = $(datadir)/icons/gnome
size = scalable
context = apps

iconsdir = $(themedir)/$(size)/$(context)
icons_DATA = rchip_client.svg

XTRA_DIST = $(png_DATA) rchip.desktop

createdirs:
	mkdir $(datadir)/madsci/

CLEANFILES = Makefile 
