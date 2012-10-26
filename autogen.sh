#!/usr/bin/env bash

PROJECT=rchip

function error () {
	echo "Error: $1" 1>&2
	exit 1
}

function check_autotool_version () {
	which $1 &>/dev/null || {
		error "$1 is not installed, and is required to configure $PACKAGE"
	}

	version=$($1 --version | head -n 1 | cut -f4 -d' ')
	major=$(echo $version | cut -f1 -d.)
	minor=$(echo $version | cut -f2 -d.)
	rev=$(echo $version | cut -f3 -d. | sed 's/[^0-9].*$//')
	major_check=$(echo $2 | cut -f1 -d.)
	minor_check=$(echo $2 | cut -f2 -d.)
	rev_check=$(echo $2 | cut -f3 -d.)

	if [ $major -lt $major_check ]; then
		do_bail=yes
	elif [[ $minor -lt $minor_check && $major = $major_check ]]; then
		do_bail=yes
	elif [[ $rev -lt $rev_check && $minor = $minor_check && $major = $major_check ]]; then
		do_bail=yes
	fi

	if [ x"$do_bail" = x"yes" ]; then
		error "$1 version $2 or better is required to configure $PROJECT"
	fi
}

function run () {
	echo "Running $@ ..."
	$@ 2>.autogen.log || {
		cat .autogen.log 1>&2
		rm .autogen.log
		error "Could not run $1, which is required to configure $PROJECT"
	}
	rm .autogen.log
}

function zwave() {
    cd lib/
    if [ -d "open-zwave" ]; then
        cd open-zwave
        svn update
    else
        svn checkout http://open-zwave.googlecode.com/svn/trunk/ open-zwave
        cd open-zwave
    fi
    cd cpp/build/linux/
    make
    cd ../../../../..
    cp lib/open-zwave/cpp/lib/linux/libopenzwave* src/
}

srcdir=$(dirname $0)
test -z "$srcdir" && srcdir=.

(test -f $srcdir/configure.ac) || {
	error "Directory \"$srcdir\" does not look like the top-level $PROJECT directory"
}

# MacPorts on OS X only seems to have glibtoolize
WHICHLIBTOOLIZE=$(which libtoolize || which glibtoolize)
if [ x"$WHICHLIBTOOLIZE" == x"" ]; then
	error "libtool is required to configure $PROJECT"
fi
LIBTOOLIZE=$(basename $WHICHLIBTOOLIZE)

zwave
check_autotool_version aclocal 1.9
check_autotool_version automake 1.9
check_autotool_version autoconf 2.53
check_autotool_version $LIBTOOLIZE 1.4.3
#for international support
#check_autotool_version intltoolize 0.35.0
check_autotool_version pkg-config 0.14.0

if git --help &>/dev/null; then
	git submodule update --init
fi

#for international suppoert
#run intltoolize --force --copy
run $LIBTOOLIZE --force --copy --automake
run aclocal -I m4
run autoconf
run autoheader

test -f config.h.in && touch config.h.in
run automake --gnu --add-missing --force --copy \
	-Wno-portability -Wno-portability

if [ ! -z "$NOCONFIGURE" ]; then
	echo "Done. ./configure skipped."
	exit $?
fi

if [ $# = 0 ]; then
	echo "WARNING: not running configure, no arguments passed."
	exit 0
fi

run ./configure $@

