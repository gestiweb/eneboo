#!/bin/bash

if [ ! -r "$1" ]
then
  echo "$0 Error: $1 no existe o no se puede acceder a él"
  exit 1
fi

INFILE="$1"
OUTFILE="$2"
ENCODE="$(file -b --mime-encoding $1)"
  
if [ "$OUTFILE" == "" ]
then
  OUTFILE="out.xml"
fi

iconv -f $ENCODE -t utf-8 ${INFILE} > ${INFILE}.tmp
xmlstarlet fo -o -e utf-8 -R $INFILE.tmp > $OUTFILE.tmp 2> /dev/null
xmlstarlet fo -e utf-8 -R $OUTFILE.tmp > $OUTFILE 2> /dev/null
rm -f $INFILE.tmp
rm -f $OUTFILE.tmp

cat $OUTFILE | xmllint --noout --nowarning - 2> tmp_$INFILE.err
DELLINES=""
for i in $(cat tmp_$INFILE.err | grep -o "^\-:[[:digit:]]\+:" | sed -n "s/\-:\(.*\):/\1d;/p")
do 
  DELLINES="$DELLINES$i"
done
rm -f tmp_$INFILE.err
sed -i "$DELLINES" $OUTFILE

xmlstarlet fo -e utf-8 -R $OUTFILE > $OUTFILE.tmp 2> /dev/null
cat $OUTFILE.tmp | xmllint --noout --nowarning -
cat $OUTFILE.tmp | sed "s/ SYSTEM \"kugartemplate.dtd\"//" > $OUTFILE
rm -f $OUTFILE.tmp

