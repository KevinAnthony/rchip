#!/bin/bash

if [ ! -f "configure.ac" ]; then
	echo "Script must be run from project root"
	exit 1
fi
sudo echo -ne
if [ -f "Makefile" ] ; then
	make maintainer-clean
	if [ "$?" -ne "0" ]; then
  		echo "1st make maintainer-clean failed"
  		exit 1
	fi	
fi
./autogen.sh $@
if [ "$?" -ne "0" ]; then
	echo "autogen.sh $@ failed"
	exit 1
fi
if [ $# = 0 ]; then
	./configure
	if [ "$?" -ne "0" ]; then
		echo "configure failed"
		exit 1
	fi
fi
make -s
if [ "$?" -ne "0" ]; then
	echo "make -s failed"
	exit 1
fi
sudo make -s install
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

