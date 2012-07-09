#!/bin/bash
SRC=src/qt
DST=eneboo-build-win32$1

mkdir "$DST" 2>/dev/null
mkdir "$DST/bin" 2>/dev/null
mkdir "$DST/lib" 2>/dev/null
mkdir "$DST/plugins" 2>/dev/null
mkdir "$DST/plugins/designer" 2>/dev/null
mkdir "$DST/plugins/styles" 2>/dev/null
mkdir "$DST/plugins/sqldrivers" 2>/dev/null

cp "$SRC/bin/"*.exe "$DST/bin"
cp "$SRC/bin/"*.dll "$DST/bin"
cp "$SRC/lib/"*.dll "$DST/lib"
cp "$SRC/plugins/designer/"*.dll "$DST/plugins/designer" 
cp "$SRC/plugins/styles/"*.dll "$DST/plugins/styles" 
cp "$SRC/plugins/sqldrivers/"*.dll "$DST/plugins/sqldrivers" 
cp -R "$SRC/share" "$DST/" 




