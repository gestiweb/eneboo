#!/bin/bash

VERSION=$(git describe --tags)
PROJECT="eneboo"
PVERSION="${PROJECT}-${VERSION}"

echo "Exportando código fuente para $PVERSION . . . "

git archive --prefix="$PVERSION/" --output="$PVERSION-src.tar" HEAD
bzip2 -9 "$PVERSION-src.tar"

mkdir "export/" 2>/dev/null
mv "$PVERSION-src.tar.bz2" "export/"


echo "codigo exportado a: export/$PVERSION-src.tar.bz2"
