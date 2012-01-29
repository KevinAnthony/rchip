SUBDIRS = src data man doc 

CLEANFILES = \
	*.tar.gz

DISTCLEANFILES = \
        Makefile \
	    *.bak \
        *~ \
        *.pidb

MAINTAINERCLEANFILES = \
        configure \
        compile \
        config.h.in \
        aclocal.m4 \
        ltmain.sh \
        Makefile.in \
        depcomp \
        missing \
        install-sh \
        config.sub \
        config.guess \
        mkinstalldirs \
		m4/libtool.m4	\
		m4/lt~obsolete.m4	\
		m4/ltoptions.m4  	\
		m4/ltsugar.m4 	\
		m4/ltversion.m4	\
		m4/pkg.m4

clean-local:
	rm -rf bin

run:
	./src/rchip

debug:
	gdb ./src/rchip
