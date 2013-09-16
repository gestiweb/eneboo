#!/bin/bash

CUR_BUILD_NUMBER="$(svnversion . -n)"

mv src/qt src/qt.tmp.backup
ln -s $PWD/../../nebula_qt3 src/qt

INST_DIR=$PWD/../../../AbanQ-Nebula

if [ "$1" != "" ]
then 
  INST_DIR=$1
fi

BUILD_NUMBER="$CUR_BUILD_NUMBER" accept="si" ./build.sh -prefix "$INST_DIR" -nebula -hoard -digidoc -platform linux-g++-32

rm -f src/qt
mv src/qt.tmp.backup src/qt

cp -f nebula.sh $INST_DIR/bin


