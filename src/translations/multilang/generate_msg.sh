#!/bin/bash

if [ ! -r "$1" ]
then
  echo "$0 Error: $1 no existe o no se puede acceder a él"
  exit 1
fi

INFILE="$1"
LANGID="$2"
TYPEID="$(echo $INFILE |awk -F . '{print $NF}')"
QTE=\&quot\;

#####
# $1 : Fichero de entrada
# $2 : Expresion XPath de los mensajes a añadir
# $3 : Expresion de captura de texto
function addMsgs() {  
  INXML="${LANGID}_in.xml"
  OUTXML="${LANGID}_out.xml"
  XPATH="$2"
  STRMATCH="$3"
              
  cp -fa "$1" $INXML
  ./validate_xml.sh "$INXML" "$OUTXML"

  xmlstarlet sel -E iso-8859-15 -T -t -m "$XPATH" \
                 -v concat\(\'translate\(${QTE}${LANGID}${QTE}\,\',\'$QTE\',string\($STRMATCH\),\'$QTE\',\'\)\;\'\) \
                 -n $OUTXML >> ${LANGID}_out.msg
                
  rm -f $INXML 
  rm -f $OUTXML
  
  echo "${LANGID} $1"
}

case "$TYPEID" in
  raw)
    addMsgs "$INFILE" "//text" ""
  ;;
  mtd)
    addMsgs "$INFILE" "//alias" "substring(.,31,string-length(.)-32)"
  ;;
  qry)
    addMsgs "$INFILE" "//alias" "substring(.,30,string-length(.)-31)"
  ;;
  kut)
    addMsgs "$INFILE" "//*[string(@Text)][name()!='Field' and @DataType!='5'][name()!='CalculatedField' or @CalculationType='6']/@Text" ""
    addMsgs "$INFILE" "//Label[@Text!='']/@Text" ""
  ;;
  ui)
    addMsgs "$INFILE" "//string[text()!='']"    ""
    addMsgs "$INFILE" "//comment[text()!='']"   ""
    addMsgs "$INFILE" "//item[@text!='']/@text" ""
  ;;
  *)
    echo "$0 Error: Tipo de fichero no reconocido $INFILE ($TYPEID)"
  ;;
esac

