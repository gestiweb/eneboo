#!/bin/bash

SUFIJO=$(/bin/date +%Y%m%d%H%M)
DIRBACKUP="./abanq-$SUFIJO/"
FILEBACKUP=abanq-$SUFIJO.tar.bz2
DESTDIR=$HOME/tmp

./clean.sh

mkdir -p $DIRBACKUP

cp -vf * $DIRBACKUP
cp -vfR src $DIRBACKUP

find $DIRBACKUP -name ".svn" -exec rm -fR \{\} \;
find $DIRBACKUP -name "CVS" -exec rm -fR \{\} \;
find $DIRBACKUP -name "*.html" -exec rm -fR \{\} \;
find $DIRBACKUP -name "iconscache" -exec rm -fR \{\} \;

tar cjvf $FILEBACKUP $DIRBACKUP

rm -fR $DIRBACKUP


#####

mv $FILEBACKUP $DESTDIR
