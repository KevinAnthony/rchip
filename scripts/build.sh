#!/bin/bash

USAGE="Usage: `basename $0` [-hvs] args"
i=0
while getopts hvsi: OPT; do
    case "$OPT" in
        h)
           	echo $USAGE
           	exit 0
            	;;
        v)
            	echo "`basename $0` version 0.1"
            	exit 0
            	;;
	s)
		silent=true
		i=$(($i+1))
		;;
	i)
		install=true
		i=$(($i+1))
		;;
        \?)
            # getopts issues an error message
            echo $USAGE >&2
            exit 1
            ;;
    esac
done
cmds=""
for cmd in $@; do
	if [ "$i" -gt "0" ]; then
		i=$(($i-1))
	else
		new=$cmds" --"$cmd
		cmds=$new
	fi
done

if [ ! -f "configure.ac" ]; then
	echo "Script must be run from project root"
	exit 1
fi
sudo echo -ne
if [ -f "Makefile" ] ; then
	if [ $silent ] ; then
		make -s maintainer-clean
	else
		make maintainer-clean
	fi
	if [ "$?" -ne "0" ]; then
  		echo "1st make maintainer-clean failed"
  		exit 1
	fi	
fi
./autogen.sh $cmds
if [ "$?" -ne "0" ]; then
	echo "autogen.sh failed"
	exit 1
fi
if [ "x$cmds" == "x" ]; then
	./configure
	if [ "$?" -ne "0" ]; then
		echo "configure failed"
		exit 1
	fi
fi
if [ $silent ]; then
        make -s
else
	make
fi
if [ "$?" -ne "0" ]; then
	echo "make failed"
	exit 1
fi
if [ $install ]; then
if [ $silent ]; then
        sudo make -s install
else
       	sudo make install
fi
if [ "$?" -ne "0" ]; then
  	echo "make -s install failed"
  	exit 1
fi
make maintainer-clean
if [ "$?" -ne "0" ]; then
        echo "2ed make maintainer-clean failed"
	exit 1
fi
git add *
if [ "$?" -ne "0" ]; then
	git reset
        echo "git add failed"
        exit 1
fi  
git commit -a
if [ "$?" -ne "0" ]; then
	git reset
        echo "git commit failed"
        exit 1
fi
git push
if [ "$?" -ne "0" ]; then
	git reset
        echo "git push failed"
        exit 1
fi

fi #if -i

