#!/bin/bash

QSFILES="$1"
QSDIR="$2"
OUTFILE="$3"

if [ "$QSFILES" == "" ]
then
  exit 1
fi
  
if [ ! -x "$QSDIR" ]
then
  echo "$0 Error: $QSDIR no existe o no se puede acceder a él"
  exit 1
fi

if [ "$OUTFILE" == "" ]
then
  OUTFILE="out.raw"
fi

echo "<texts>" > $OUTFILE
for q in $QSFILES
do
  for i in $(ls $QSDIR/$q)
  do
    if [ -r "$i" ]
    then
      grep -o 'translate[[:space:]]*([[:space:]]*"[^"]*\(\\.\)*[^"]*"[[:space:]]*,[[:space:]]*"[^"]*\(\\.\)*[^"]*"[[:space:]]*)' $i | \
      sed -n 's/translate[[:space:]]*([[:space:]]*"[^"]*\(\\.\)*[^"]*"[[:space:]]*,[[:space:]]*"\([^"]*\(\\.\)*[^"]*\)"[[:space:]]*)/\<text\>\2\<\/text\>/p' >> $OUTFILE
    else
      echo "$0 Error: $i no existe o no se puede acceder a él"
    fi
  done
done
echo "</texts>" >> $OUTFILE

