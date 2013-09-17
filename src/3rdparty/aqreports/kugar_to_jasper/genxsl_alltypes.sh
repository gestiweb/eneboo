#!/bin/bash

xmlstarlet sel -C -t \
-e "jasperReport" \
-m "//CalculatedField[@FunctionName='flfactinfo.pub_traducirEtiqueta']" \
-e "staticText" \
-e "reportElement" -a "x" -i "number(@X)>20" -v "string(number(@X)-20)" -b -i "21>number(@X)" -v "@X" -b -b \
                   -a "y" -v "@Y" -b -a "width" -v "@Width" -b -a "height" -v "@Height" -b -b \
-e "textElement" -a "textAlignment" -i "@HAlignment='0'" -o "Left" -b -i "@HAlignment='1'" -o "Center" -b -i "@HAlignment='2'" -o "Right" -b -b \
                 -a "verticalAlignment" -i "@VAlignment='0'" -o "Top" -b -i "@VAlignment='1'" -o "Middle" -b -i "@VAlignment='2'" -o "Bottom" -b -b \
-e "font" -a "fontName" -v "@FontFamily" -b -i "number(@FontWeight)>50" -a "isBold" -o "true" -b -b -b -b \
-e "text" -o "&lt;![CDATA[" -v "@Text" -o "]]&gt;" -b -b -b \
-m "//Field[@Field!='']" \
-e "textField" \
-e "reportElement" -a "x" -i "number(@X)>20" -v "string(number(@X)-20)" -b -i "21>number(@X)" -v "@X" -b -b \
                   -a "y" -v "@Y" -b -a "width" -v "@Width" -b -a "height" -v "@Height" -b -b \
-e "textElement" -a "textAlignment" -i "@HAlignment='0'" -o "Left" -b -i "@HAlignment='1'" -o "Center" -b -i "@HAlignment='2'" -o "Right" -b -b \
                 -a "verticalAlignment" -i "@VAlignment='0'" -o "Top" -b -i "@VAlignment='1'" -o "Middle" -b -i "@VAlignment='2'" -o "Bottom" -b -b \
-e "font" -a "fontName" -v "@FontFamily" -b -i "number(@FontWeight)>50" -a "isBold" -o "true" -b -b -b -b \
-e "textFieldExpression" -a "class" -i "@DataType='0'" -o "java.lang.String" -b \
                                    -i "@DataType='1'" -o "java.lang.Integer" -b \
                                    -i "@DataType='2'" -o "java.lang.Float" -b \
                                    -i "@DataType='3'" -o "java.util.Date" -b \
                                    -i "@DataType='4'" -o "java.lang.Number" -b \
                                    -i "@DataType='5'" -o "java.lang.String" -b \
                                    -i "@DataType='6'" -o "java.lang.String" -b \
                                    -i "@DataType='7'" -o "java.lang.Boolean" -b \
                         -b -o "&lt;![CDATA[\$F{" -v "@Field" -o "}]]&gt;" -b -b -b \
-m "//CalculatedField[not(@FunctionName) and @Field!='']" \
-e "textField" \
-e "reportElement" -a "x" -i "number(@X)>20" -v "string(number(@X)-20)" -b -i "21>number(@X)" -v "@X" -b -b \
                   -a "y" -v "@Y" -b -a "width" -v "@Width" -b -a "height" -v "@Height" -b -b \
-e "textElement" -a "textAlignment" -i "@HAlignment='0'" -o "Left" -b -i "@HAlignment='1'" -o "Center" -b -i "@HAlignment='2'" -o "Right" -b -b \
                 -a "verticalAlignment" -i "@VAlignment='0'" -o "Top" -b -i "@VAlignment='1'" -o "Middle" -b -i "@VAlignment='2'" -o "Bottom" -b -b \
-e "font" -a "fontName" -v "@FontFamily" -b -i "number(@FontWeight)>50" -a "isBold" -o "true" -b -b -b -b \
-e "textFieldExpression" -a "class" -i "@DataType='0'" -o "java.lang.String" -b \
                                    -i "@DataType='1'" -o "java.lang.Integer" -b \
                                    -i "@DataType='2'" -o "java.lang.Float" -b \
                                    -i "@DataType='3'" -o "java.util.Date" -b \
                                    -i "@DataType='4'" -o "java.lang.Number" -b \
                                    -i "@DataType='5'" -o "java.lang.String" -b \
                                    -i "@DataType='6'" -o "java.lang.String" -b \
                                    -i "@DataType='7'" -o "java.lang.Boolean" -b \
                         -b -o "&lt;![CDATA[\$F{calc_" -v "@Field" -o "}]]&gt;" -b -b -b \
out.xml > sout.xsl

