#!/bin/bash

if [ -d src/qt.tmp.backup ]
then
  rm -f src/qt
  mv src/qt.tmp.backup src/qt
fi

mv src/qt src/qt.tmp.backup
ln -s $PWD/../../nebula_qt3 src/qt

./clean.sh

rm -f src/qt
mv src/qt.tmp.backup src/qt

CURDIR=$PWD
cd ../../nebula_qt3
find . -name "Makefile" -exec rm -vf \{\} \;
find . -name ".moc" -type d -exec rm -vfR \{\} \;
find . -name ".obj" -type d -exec rm -vfR \{\} \;
find . -name ".uic" -type d -exec rm -vfR \{\} \;
find . -name ".tmp" -type d -exec rm -vfR \{\} \;
find . -name "*.qm" -exec rm -vf \{\} \;
find . -name "*.o" -exec rm -vf \{\} \;
find . -name "*.moc" -exec rm -vf \{\} \;
find . -name ".*.cache" -exec rm -vf \{\} \;
find . -name "*.prl" -exec rm -vf \{\} \;
find . -name "*.*.orig" -exec rm -vf \{\} \;
cd $CURDIR

find . -type d -name "home" -exec rm -fr \{\} \;
svn up

