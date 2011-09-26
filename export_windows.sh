#!/bin/bash

VERSION=$(git describe --tags)
PROJECT="eneboo"
PVERSION="${PROJECT}-${VERSION}"
SRC="eneboo-build-win32"
test -e "$SRC" || { echo "No existe compilacion para Windows 32 bits! (falta carpeta $SRC)"; exit 1; }

echo "Exportando compilacion Windows 32 bits para $PVERSION . . . "

mv "$SRC" "$PVERSION"
tar cf "$PVERSION-win32.tar" "$PVERSION"
bzip2 -9 "$PVERSION-win32.tar"

mkdir "export/" 2>/dev/null
cp "$PVERSION-win32.tar.bz2" "export/"
unlink "$PVERSION-win32.tar.bz2"
rm -Rf "$PVERSION"

echo "Compilacion exportada a: export/$PVERSION-win32.tar.bz2"
