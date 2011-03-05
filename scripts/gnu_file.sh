#/usr/bin/env bash

function error () {
        echo "Error: $1" 1>&2
        exit 1
}


file=$1;

if [ ! -f $file ]; then
	error "$file does not exist"
fi

sed -e 's/        /\t/g' -e 's/       //g' -e 's/      //g' -e 's/     //g' -e 's/    //g' -e 's/   //g' -e 's/  //' $file > $file.out
mv $file.out $file
