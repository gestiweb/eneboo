#!/bin/bash

VERSION=$(git describe --tags)
PROJECT="eneboo"
PVERSION="${PROJECT}-${VERSION}-quick"
SRC="eneboo-build-linux32-quick"
test -e "$SRC" || { echo "No existe compilacion Quick para Linux 32 bits! (falta carpeta $SRC)"; exit 1; }

echo "Exportando compilacion Quick Linux 32 bits para $PVERSION . . . "

mv "$SRC" "$PVERSION"
tar cf "$PVERSION-linux32.tar" "$PVERSION" --exclude="*.a" --exclude="*.o" --exclude="*.prl" --exclude="mkspecs" --exclude="include"  --exclude="templates" 
bzip2 -9 "$PVERSION-linux32.tar"

mkdir "export/" 2>/dev/null

cp "$PVERSION-linux32.tar.bz2" "export/"
unlink "$PVERSION-linux32.tar.bz2"
rm "$PVERSION" -Rf


echo "Compilación exportada a: export/$PVERSION-linux32.tar.bz2"
