#!/bin/bash

VERSION=$(git describe --tags $1 )
PROJECT="eneboo"
PVERSION="${PROJECT}-${VERSION}"

echo "Exportando código fuente para $PVERSION . . . "
git log -1 "$VERSION"
git archive --prefix="$PVERSION/" --output="$PVERSION-src.tar" "$VERSION" || exit 1
bzip2 -1 "$PVERSION-src.tar"

mkdir "export/" 2>/dev/null
mv "$PVERSION-src.tar.bz2" "export/"


echo "codigo exportado a: export/$PVERSION-src.tar.bz2"
