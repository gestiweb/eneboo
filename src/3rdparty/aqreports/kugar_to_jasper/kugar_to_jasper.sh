#!/bin/bash

if [ "$1" == "" ]
then
  echo "Uso: $0 kugar_template.kut"
  echo "Salida: kugar_template.kut_to_jasper_tags.xml"
  exit 1
fi

./validate_xml.sh $1

./genxsl.sh

cat sout.xsl | sed "s/\(\[CDATA\[[^\"].*\"\)/\1 disable-output-escaping=\"yes\"/" > sout.xsl.tmp
mv -f sout.xsl.tmp sout.xsl
cat sout.xsl | sed "s/\(\]\]\&gt\;[^\"].*\"\)/\1 disable-output-escaping=\"yes\"/" > sout.xsl.tmp
mv -f sout.xsl.tmp sout.xsl

xmlstarlet tr sout.xsl out.xml | xmlstarlet fo -s 2 > ${1}_to_jasper_tags.xml

rm -f out.xml 
rm -f sout.xsl

echo ""
echo "Generado -> ${1}_to_jasper_tags.xml"
echo ""

