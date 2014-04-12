#!/bin/bash

VERSION=$(git describe --tags)
PROJECT="eneboo"
PVERSION="${PROJECT}-${VERSION}-quick"
SRC="eneboo-build-linux64-quick"
test -e "$SRC" || { echo "No existe compilacion Quick para Linux 64 bits! (falta carpeta $SRC)"; exit 1; }

echo "Exportando compilacion Quick Linux 64 bits para $PVERSION . . . "

mv "$SRC" "$PVERSION"
tar cf "$PVERSION-linux64.tar" "$PVERSION"
bzip2 -9 "$PVERSION-linux64.tar"

mkdir "export/" 2>/dev/null
mv "$PVERSION-linux64.tar.bz2" "export/"
#unlink "$PVERSION-linux64.tar.bz2"
#rm "$PVERSION" -Rf



echo "Compilación exportada a: export/$PVERSION-linux64.tar.bz2"
