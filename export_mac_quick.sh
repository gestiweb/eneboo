#!/bin/bash

VERSION=$(git describe --tags)
PROJECT="eneboo"
PVERSION="${PROJECT}-${VERSION}-quick"
SRC="eneboo-build-macx-quick"
test -e "$SRC" || { echo "No existe compilación Quick para MacOSX ! (falta carpeta $SRC)"; exit 1; }

echo "Exportando compilación Quick MacOSX para $PVERSION . . . "

mv "$SRC" "$PVERSION"

rm -Rf "$PVERSION/lib/*.a"
rm -Rf "$PVERSION/lib/*.o"
rm -Rf "$PVERSION/lib/*.prl"
rm -Rf "$PVERSION/mkspecs"
rm -Rf "$PVERSION/include"
rm -Rf "$PVERSION/templates"

tar cf "$PVERSION-macOSX.tar" "$PVERSION" 
bzip2 -9 "$PVERSION-macOSX.tar"

mkdir "export/" 2>/dev/null

cp "$PVERSION-macOSX.tar.bz2" "export/"
unlink "$PVERSION-macOSX.tar.bz2"
rm -Rf "$PVERSION" 


echo "Compilación exportada a: export/$PVERSION-linux32.tar.bz2"
