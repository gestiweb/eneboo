#!/bin/bash

export QTDIR=/usr/share/qt4
export EXE_QMAKE=$QTDIR/bin/qmake
export EXE_MAKE="/usr/bin/make -s"

if [ "$1" == "wui" ]
then
	echo "Build AbanQ WUI Sumiller"
	cd build/wt
	make install
	cd ../..
	echo "CONFIG += aqwui console no_keywords" > ./gen_settings.pri
	echo "DEFINES += AQ_WUI" >> ./gen_settings.pri
	echo "AQUISRC_DIR = wui" >> ./gen_settings.pri
	$EXE_QMAKE -r "CONFIG += debug_and_release" abanq.pro
else
	echo "Build AbanQ GUI Sumiller"
	echo "CONFIG += aqgui" > ./gen_settings.pri
	echo "DEFINES += AQ_GUI" >> ./gen_settings.pri
	echo "AQUISRC_DIR = gui" >> ./gen_settings.pri
	$EXE_QMAKE -r "CONFIG += debug_and_release" abanq.pro
fi

$EXE_MAKE copyheaders
