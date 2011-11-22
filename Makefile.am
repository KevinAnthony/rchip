SUBDIRS = src data man doc m4

CLEANFILES = \
	*.tar.gz

DISTCLEANFILES = \
        Makefile \
	*.bak \
        *~ \
        *.pidb

MAINTAINERCLEANFILES = \
        compile \
        INSTALL \
        config.h.in \
        aclocal.m4 \
        ltmain.sh \
        Makefile.in \
        depcomp \
        missing \
        install-sh \
        config.sub \
        config.guess \
        mkinstalldirs

clean-local:
	rm -rf bin

run:
	./src/rchip

debug:
	gdb ./src/rchip
