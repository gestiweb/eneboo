#!/bin/bash

xmlstarlet sel -C -t \
-e "jasperReport" \
-m "//CalculatedField[@CalculationType='6' and not(@FunctionName)]" \
-e "staticText" \
-e "reportElement" -a "x" -i "number(@X)>9" -v "string(number(@X)-10)" -b -i "10>number(@X)" -v "0" -b -b \
                   -a "y" -v "@Y" -b -a "width" -v "@Width" -b -a "height" -v "@Height" -b -b \
-e "textElement" -a "textAlignment" -i "@HAlignment='0'" -o "Left" -b -i "@HAlignment='1'" -o "Center" -b -i "@HAlignment='2'" -o "Right" -b -b \
                 -a "verticalAlignment" -i "@VAlignment='0'" -o "Top" -b -i "@VAlignment='1'" -o "Middle" -b -i "@VAlignment='2'" -o "Bottom" -b -b \
-e "font" -a "fontName" -v "@FontFamily" -b -i "number(@FontWeight)>50" -a "isBold" -o "true" -b -b -b -b \
-e "text" -o "&lt;![CDATA[" -v "@Text" -o "]]&gt;" -b -b -b \
-m "//Label" \
-e "staticText" \
-e "reportElement" -a "x" -i "number(@X)>9" -v "string(number(@X)-10)" -b -i "10>number(@X)" -v "0" -b -b \
                   -a "y" -v "@Y" -b -a "width" -v "@Width" -b -a "height" -v "@Height" -b -b \
-e "textElement" -a "textAlignment" -i "@HAlignment='0'" -o "Left" -b -i "@HAlignment='1'" -o "Center" -b -i "@HAlignment='2'" -o "Right" -b -b \
                 -a "verticalAlignment" -i "@VAlignment='0'" -o "Top" -b -i "@VAlignment='1'" -o "Middle" -b -i "@VAlignment='2'" -o "Bottom" -b -b \
-e "font" -a "fontName" -v "@FontFamily" -b -i "number(@FontWeight)>50" -a "isBold" -o "true" -b -b -b -b \
-e "text" -o "&lt;![CDATA[" -v "@Text" -o "]]&gt;" -b -b -b \
-m "//CalculatedField" \
-e "textField" \
-e "reportElement" -a "x" -i "number(@X)>9" -v "string(number(@X)-10)" -b -i "10>number(@X)" -v "0" -b -b \
                   -a "y" -v "@Y" -b -a "width" -v "@Width" -b -a "height" -v "@Height" -b -b \
-e "textElement" -a "textAlignment" -i "@HAlignment='0'" -o "Left" -b -i "@HAlignment='1'" -o "Center" -b -i "@HAlignment='2'" -o "Right" -b -b \
                 -a "verticalAlignment" -i "@VAlignment='0'" -o "Top" -b -i "@VAlignment='1'" -o "Middle" -b -i "@VAlignment='2'" -o "Bottom" -b -b \
-e "font" -a "fontName" -v "@FontFamily" -b -i "number(@FontWeight)>50" -a "isBold" -o "true" -b -b -b -b \
-e "textFieldExpression" -a "class" -o "java.lang.String" -b \
                         -o "&lt;![CDATA[\$F{" -v "@Field" -o "}]]&gt;" -b -b -b \
-m "//Field" \
-e "textField" \
-e "reportElement" -a "x" -i "number(@X)>9" -v "string(number(@X)-10)" -b -i "10>number(@X)" -v "0" -b -b \
                   -a "y" -v "@Y" -b -a "width" -v "@Width" -b -a "height" -v "@Height" -b -b \
-e "textElement" -a "textAlignment" -i "@HAlignment='0'" -o "Left" -b -i "@HAlignment='1'" -o "Center" -b -i "@HAlignment='2'" -o "Right" -b -b \
                 -a "verticalAlignment" -i "@VAlignment='0'" -o "Top" -b -i "@VAlignment='1'" -o "Middle" -b -i "@VAlignment='2'" -o "Bottom" -b -b \
-e "font" -a "fontName" -v "@FontFamily" -b -i "number(@FontWeight)>50" -a "isBold" -o "true" -b -b -b -b \
-e "textFieldExpression" -a "class" -o "java.lang.String" -b \
                         -o "&lt;![CDATA[\$F{" -v "@Field" -o "}]]&gt;" -b -b -b \
-m "//Line" \
-e "line" \
-e "reportElement" -a "x" -i "number(@X1)>9" -v "string(number(@X1)-10)" -b -i "10>number(@X1)" -v "0" -b -b \
                   -a "y" -v "@Y1" -b -a "width" -v "number(@X2)-number(@X1)" -b -a "height" -v "number(@Y2)-number(@Y1)" -b -b \
out.xml > sout.xsl

