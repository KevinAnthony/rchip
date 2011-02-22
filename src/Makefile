SHELL = /bin/sh

PREFIX=/usr

GOB1=gob2
CC=gcc
LD=gcc

DEPS=gtk+-3.0 dbus-1 dbus-glib-1 glib-2.0 libxml-2.0 gconf-2.0
CFLAGS=$(shell pkg-config --cflags $(DEPS)) 
CFLAGS+=$(shell mysql_config --cflags)
OFLAGS=-std=c99 -Wall -g -D _DEBUG -D 'PREFIX="$(PREFIX)"'
#CFLAGS+=-std=c99 -Wall -g -D _DEBUG -D _NOSQL -D 'PREFIX="$(PREFIX)"'
#CFLAGS+=-std=c99 -Wall -g -D _SILENT -D 'PREFIX="$(PREFIX)"'

LDFLAGS=$(shell pkg-config --libs $(DEPS)) 
LDFLAGS+=$(shell mysql_config --libs )

OBJS=msdaemon.o utils.o mstray.o notifications.o sql.o dbus.o cmdhandler.o showlist.o settings.o

PROG=msdaemon

DATA=../data/*.png
LAUNCHER = ../data/msdaemon.desktop
ICON=../data/msdaemon.svg
INSTALL_BIN = $(PREFIX)/sbin
INSTALL_DATA = $(PREFIX)/share/madsci
INSTALL_ICON = /usr/share/icons/hicolor/scalable/apps
INSTALL_LAUNCHER = /usr/local/share/applications/
 
INC=./*.h

%.c %.h : %.gob
	$(GOB2) $<

$(PROG): $(OBJS)
	$(CC) $(OFLAGS) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $(PROG)
	$(shell echo)

%.o: %.c
	$(CC) $(OFLAGS) $(CFLAGS) $(LDFLAGS) -c $< $(INC) 
	$(shell echo)

all: $(PROG)

default: $(PROG)

install: all
	mkdir -p $(INSTALL_BIN)
	cp $(PROG) $(INSTALL_BIN)
	mkdir -p $(INSTALL_DATA)
	cp $(DATA) $(INSTALL_DATA)
	mkdir -p $(INSTALL_ICON)
	
	mkdir -p $(INSTALL_LAUNCHER)
	cp $(LAUNCHER) $(INSTALL_LAUNCHER)

clean:
	rm -f *.o $(PROG) *.gch 
