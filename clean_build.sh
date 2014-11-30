#!/bin/bash
BASEDIR=$PWD

 if [ -f $1 ]; then
 echo "No se encuentra la carpeta $1"
 echo "Proceso terminado"
 exit 1
 fi


if [ "$1" == "eneboo-build-win32-dba" -o "$1" == "eneboo-build-win32-quick" -o "$1" == "eneboo-build-win64-dba" -o "$1" == "eneboo-build-win64-quick" ];then
echo "Crenando carpetas para compilación windows ..."
mkdir $1
cp -Rf $BASEDIR/src/qt/bin $1
### El ejecutable para windows no usa esta carpeta
#cp -Rf $BASEDIR/src/qt/lib $1 
cp -Rf $BASEDIR/src/qt/plugins $1
cp -Rf $BASEDIR/src/qt/share $1
fi

if [ "$1" == "eneboo-build-mac_i686-quick" -o "$1" == "eneboo-build-mac_i686-dba" -o "$1" == "eneboo-build-mac_PPC-quick" -o "$1" == "eneboo-build-mac_PPC-dba" ];then
echo "Crenando carpetas para compilación MacOS ..."
mkdir $1
cp -Rf $BASEDIR/src/qt/bin $1
cp -Rf $BASEDIR/src/qt/lib $1
cp -Rf $BASEDIR/src/qt/plugins $1
cp -Rf $BASEDIR/src/qt/share $1
fi

echo "Accediendo a $1 ..."
cd $1



echo "Borrando carpetas ..."
#Linux
rm -Rf ./include
rm -Rf ./mkspecs
rm -Rf ./phrasebooks
rm -Rf ./templates
rm -Rf ./share/eneboo/images
rm -Rf ./share/eneboo/tmp
rm -Rf ./share/eneboo/doc
rm -Rf ./share/eneboo/reports
rm -Rf ./share/eneboo/queries

#MacOS
rm -Rf ./plugins/gfxdrivers
rm -Rf ./plugins/src
rm -Rf ./lib/fonts

#Win
rm -Rf ./plugins/accessible
rm -Rf ./plugins/codecs
rm -Rf ./plugins/imageformats

echo "Borrando ficheros ..."
rm -f ./bin/dbf*
rm -f ./bin/lupdate*
rm -f ./bin/moc*
rm -f ./bin/qmake*
rm -f ./bin/uic*

#Win
rm -f ./plugins/designer*.a
rm -f ./plugins/sqldrivers*.a
rm -f ./plugins/styles*.a

rm -f ./lib/*.a
rm -f ./lib/*.prl
rm -f ./lib/README

echo "Copiando Licencia ..."
cp $BASEDIR/COPYING .
echo "Proceso terminado"
