#!/bin/bash

source ./multilang.def

if [ ! -d "$MODDIR" -o ! -x "$MODDIR" ]
then
  echo "$0 Error: $MODDIR No existe o no se puede acceder a él"
  exit 1
fi

if [ ! -x "./generate_msg.sh" -o ! -x "./validate_xml.sh" ]
then
  echo "$0 Error: No encontrado ./generate_msg.sh o ./validate_xml.sh"
  exit 1
fi

#####
# $1 : Lista de Ficheros
# $2 : Directorio de los ficheros
# $3 : Identificador del idioma
function genMsgs() {
  FILES="$1"
  DIRFILES="$2"
  LANGID="$3"
  
  if [ "$FILES" != "" -a -x "$DIRFILES" ]
  then
    for f in $FILES
    do
      for i in $(ls $DIRFILES/$f)
      do
        ./generate_msg.sh "$i" "${LANGID}_MULTILANG"
      done
    done
  fi
}

MSGFILES=""

for l in $LANGS
do
  l=$(echo $l | awk '{ print toupper($1) }')
  
  genMsgs "$TEXTS"      "./"              "${l}"
  genMsgs "$METADATAS"  "$MODDIR/tables"  "${l}"
  genMsgs "$QUERIES"    "$MODDIR/queries" "${l}"
  genMsgs "$REPORTS"    "$MODDIR/reports" "${l}"
  genMsgs "$FORMS"      "$MODDIR/forms"   "${l}"
  
  if [ "$SCRIPTS" != "" ]
  then
    ./fetch_qs_msgs_raw.sh "$SCRIPTS" "$MODDIR/scripts"
    if [ -r "out.raw" ]
    then
      genMsgs "out.raw" "./" "${l}"
      rm -f out.raw
    fi
  fi
  
  if [ -r "${l}_MULTILANG_out.msg" ]
  then
    MSGFILES="$MSGFILES ${l}_MULTILANG_out.msg"
  fi
done

OUTFILE="$(basename $MODDIR/*.mod | sed -n 's/\(.*\)\.mod/\1/p').multilang.ts"

if [ "$MSGFILES" != "" ]
then
  lupdate -verbose $MSGFILES -ts ../$OUTFILE
  rm -f $MSGFILES
fi

