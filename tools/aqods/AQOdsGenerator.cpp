/***************************************************************************
 AQOdsGenerator.cpp
 -------------------
 begin                : 16/12/2011
 copyright            : (C) 2003-2011 by InfoSiAL S.L.
 email                : mail@infosial.com
 ***************************************************************************/
/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
 ***************************************************************************/
/***************************************************************************
 Este  programa es software libre. Puede redistribuirlo y/o modificarlo
 bajo  los  términos  de  la  Licencia  Pública General de GNU   en  su
 versión 2, publicada  por  la  Free  Software Foundation.
 ***************************************************************************/

#include "AQOdsGenerator.h"
#include "AQS_p.h"
#include "AQZipWriter.h"
#include "FLDiskCache.h"


QString data_main("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<xsl:stylesheet \n"
  "version=\"1.0\" \n"
  "xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\" \n"
  "xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\"\n" 
  "xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\" \n"
  "xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" \n"
  "xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" \n"
  "xmlns:draw=\"urn:oasis:names:tc:opendocument:xmlns:drawing:1.0\" \n"
  "xmlns:fo=\"urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0\" \n"
  "xmlns:xlink=\"http://www.w3.org/1999/xlink\" \n"
  "xmlns:dc=\"http://purl.org/dc/elements/1.1/\" \n"
  "xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" \n"
  "xmlns:number=\"urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0\" \n"
  "xmlns:presentation=\"urn:oasis:names:tc:opendocument:xmlns:presentation:1.0\"\n"
  "xmlns:svg=\"urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0\" \n"
  "xmlns:chart=\"urn:oasis:names:tc:opendocument:xmlns:chart:1.0\" \n"
  "xmlns:dr3d=\"urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0\" \n"
  "xmlns:math=\"http://www.w3.org/1998/Math/MathML\" \n"
  "xmlns:form=\"urn:oasis:names:tc:opendocument:xmlns:form:1.0\" \n"
  "xmlns:script=\"urn:oasis:names:tc:opendocument:xmlns:script:1.0\" \n"
  "xmlns:ooo=\"http://openoffice.org/2004/office\" \n"
  "xmlns:ooow=\"http://openoffice.org/2004/writer\" \n"
  "xmlns:oooc=\"http://openoffice.org/2004/calc\" \n"
  "xmlns:dom=\"http://www.w3.org/2001/xml-events\" \n"
  "xmlns:xforms=\"http://www.w3.org/2002/xforms\" \n"
  "xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" \n"
  "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" \n"
  "xmlns:rpt=\"http://openoffice.org/2005/report\" \n"
  "xmlns:of=\"urn:oasis:names:tc:opendocument:xmlns:of:1.2\" \n"
  "xmlns:rdfa=\"http://docs.oasis-open.org/opendocument/meta/rdfa#\"\n"
  "xmlns:field=\"urn:openoffice:names:experimental:ooxml-odf-interop:xmlns:field:1.0\" \n"
  "xmlns:formx=\"urn:openoffice:names:experimental:ooxml-odf-interop:xmlns:form:1.0\" \n"
  "xmlns=\"http://www.w3.org/1999/xhtml\">\n"
"<!-- See \"Extracting Unique Values with XSL\" \n"
     "(http://www.bernzilla.com/item.php?id=333) -->\n"
"<!-- xsl:key name=\"cell-styles\" match=\"cell\" use=\"@style\"/ -->\n"
"<xsl:output method=\"xml\" indent=\"yes\" encoding=\"UTF-8\"/>\n"
"<xsl:template match=\"/spreadsheet\">\n"
"<office:document-content \n"
  "xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" \n"
  "xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\" \n"
  "xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" \n"
  "xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" \n"
  "xmlns:draw=\"urn:oasis:names:tc:opendocument:xmlns:drawing:1.0\" \n"
  "xmlns:fo=\"urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0\" \n"
  "xmlns:xlink=\"http://www.w3.org/1999/xlink\" \n"
  "xmlns:dc=\"http://purl.org/dc/elements/1.1/\" \n"
  "xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" \n"
  "xmlns:number=\"urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0\" \n"
  "xmlns:presentation=\"urn:oasis:names:tc:opendocument:xmlns:presentation:1.0\"\n"
  "xmlns:svg=\"urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0\" \n"
  "xmlns:chart=\"urn:oasis:names:tc:opendocument:xmlns:chart:1.0\" \n"
  "xmlns:dr3d=\"urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0\" \n"
  "xmlns:math=\"http://www.w3.org/1998/Math/MathML\" \n"
  "xmlns:form=\"urn:oasis:names:tc:opendocument:xmlns:form:1.0\" \n"
  "xmlns:script=\"urn:oasis:names:tc:opendocument:xmlns:script:1.0\" \n"
  "xmlns:ooo=\"http://openoffice.org/2004/office\" \n"
  "xmlns:ooow=\"http://openoffice.org/2004/writer\" \n"
  "xmlns:oooc=\"http://openoffice.org/2004/calc\" \n"
  "xmlns:dom=\"http://www.w3.org/2001/xml-events\" \n"
  "xmlns:xforms=\"http://www.w3.org/2002/xforms\" \n"
  "xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" \n"
  "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" \n"
  "xmlns:rpt=\"http://openoffice.org/2005/report\" \n"
  "xmlns:of=\"urn:oasis:names:tc:opendocument:xmlns:of:1.2\" \n"
  "xmlns:rdfa=\"http://docs.oasis-open.org/opendocument/meta/rdfa#\"\n"
  "xmlns:field=\"urn:openoffice:names:experimental:ooxml-odf-interop:xmlns:field:1.0\" \n"
  "xmlns:formx=\"urn:openoffice:names:experimental:ooxml-odf-interop:xmlns:form:1.0\" \n"
  "office:version=\"1.2\">\n"
"<office:automatic-styles>\n"
  "<xsl:for-each select=\"//cell[@style!='' or @bgcolor!='' or @fgcolor!='']\">\n"
  "<!-- xsl:for-each select=\"//cell[generate-id() = generate-id(key('cell-styles',@style)[1])]\" -->\n"
    "<style:style style:name=\"cell-style-{@style}BG{@bgcolor}FG{@fgcolor}\" \n"
                 "style:family=\"table-cell\">\n"
      "<style:table-cell-properties>\n"
        "<xsl:if test=\"@bgcolor\">\n"
          "<xsl:attribute name=\"fo:background-color\">\n"
            "<xsl:value-of select=\"@bgcolor\"/>\n"
           "</xsl:attribute>\n"
        "</xsl:if>\n"
        "<xsl:choose>\n"
          "<xsl:when test=\"contains(@style,'border-bottom')\">\n"
            "<xsl:attribute name=\"fo:border-bottom\">\n"
              "<xsl:text>0.0008in solid #000000</xsl:text>\n"
            "</xsl:attribute>\n"
          "</xsl:when>\n"
          "<xsl:otherwise>\n"
            "<xsl:attribute name=\"fo:border-bottom\">\n"
              "<xsl:text>none</xsl:text>\n"
            "</xsl:attribute>\n"
          "</xsl:otherwise>\n"
        "</xsl:choose>\n"
        "<xsl:choose>\n"
          "<xsl:when test=\"contains(@style,'border-left')\">\n"
            "<xsl:attribute name=\"fo:border-left\">\n"
              "<xsl:text>0.0008in solid #000000</xsl:text>\n"
            "</xsl:attribute>\n"
          "</xsl:when>\n"
          "<xsl:otherwise>\n"
            "<xsl:attribute name=\"fo:border-left\">\n"
              "<xsl:text>none</xsl:text>\n"
            "</xsl:attribute>\n"
          "</xsl:otherwise>\n"
        "</xsl:choose>\n"
        "<xsl:choose>\n"
          "<xsl:when test=\"contains(@style,'border-right')\">\n"
            "<xsl:attribute name=\"fo:border-right\">\n"
              "<xsl:text>0.0008in solid #000000</xsl:text>\n"
            "</xsl:attribute>\n"
          "</xsl:when>\n"
          "<xsl:otherwise>\n"
            "<xsl:attribute name=\"fo:border-right\">\n"
              "<xsl:text>none</xsl:text>\n"
            "</xsl:attribute>\n"
          "</xsl:otherwise>\n"
        "</xsl:choose>\n"
        "<xsl:choose>\n"
          "<xsl:when test=\"contains(@style,'border-top')\">\n"
            "<xsl:attribute name=\"fo:border-top\">\n"
              "<xsl:text>0.0008in solid #000000</xsl:text>\n"
            "</xsl:attribute>\n"
          "</xsl:when>\n"
          "<xsl:otherwise>\n"
            "<xsl:attribute name=\"fo:border-top\">\n"
              "<xsl:text>none</xsl:text>\n"
            "</xsl:attribute>\n"
          "</xsl:otherwise>\n"
        "</xsl:choose>\n"
      "</style:table-cell-properties>\n"
      "<style:paragraph-properties>\n"
        "<xsl:attribute name=\"fo:text-align\">\n"
          "<xsl:choose>\n"
            "<xsl:when test=\"contains(@style,'align-left')\">\n"
              "<xsl:text>begin</xsl:text>\n"
            "</xsl:when>\n"
            "<xsl:when test=\"contains(@style,'align-center')\">\n"
              "<xsl:text>center</xsl:text>\n"
            "</xsl:when>\n"
            "<xsl:when test=\"contains(@style,'align-right')\">\n"
              "<xsl:text>end</xsl:text>\n"
            "</xsl:when>\n"
          "</xsl:choose>\n"
        "</xsl:attribute>\n"
      "</style:paragraph-properties>\n"
      "<style:text-properties>\n"
        "<xsl:if test=\"@fgcolor\">\n"
          "<xsl:attribute name=\"fo:color\">\n"
            "<xsl:value-of select=\"@fgcolor\"/>\n"
          "</xsl:attribute>\n"
        "</xsl:if>\n"
        "<xsl:if test=\"contains(@style,'text-bold')\">\n"
          "<xsl:attribute name=\"fo:font-weight\">\n"
            "<xsl:text>bold</xsl:text>\n"
          "</xsl:attribute>\n"
          "<xsl:attribute name=\"style:font-weight-asian\">\n"
            "<xsl:text>bold</xsl:text>\n"
          "</xsl:attribute>\n"
          "<xsl:attribute name=\"style:font-weight-complex\">\n"
            "<xsl:text>bold</xsl:text>\n"
          "</xsl:attribute>\n"
        "</xsl:if>\n"
        "<xsl:if test=\"contains(@style,'text-italic')\">\n"
          "<xsl:attribute name=\"fo:font-style\">\n"
            "<xsl:text>italic</xsl:text>\n"
          "</xsl:attribute>\n"
          "<xsl:attribute name=\"style:font-style-asian\">\n"
            "<xsl:text>italic</xsl:text>\n"
          "</xsl:attribute>\n"
          "<xsl:attribute name=\"style:font-style-complex\">\n"
            "<xsl:text>italic</xsl:text>\n"
          "</xsl:attribute>\n"
        "</xsl:if>\n"
        "<xsl:if test=\"contains(@style,'text-underline')\">\n"
          "<xsl:attribute name=\"style:text-underline-style\">\n"
            "<xsl:text>solid</xsl:text>\n"
          "</xsl:attribute>\n"
          "<xsl:attribute name=\"style:text-underline-width\">\n"
            "<xsl:text>auto</xsl:text>\n"
          "</xsl:attribute>\n"
          "<xsl:attribute name=\"style:text-underline-color\">\n"
            "<xsl:text>font-color</xsl:text>\n"
          "</xsl:attribute>\n"
        "</xsl:if>\n"
      "</style:text-properties>\n"
    "</style:style>\n"
  "</xsl:for-each>\n"
"</office:automatic-styles>\n"
"<office:body>\n"
"<office:spreadsheet>\n"
"<xsl:apply-templates select=\"sheet\"/>\n"
"</office:spreadsheet>\n"
"</office:body>\n"
"</office:document-content>\n"
"</xsl:template>\n"

"<xsl:template match=\"sheet\">\n"
  "<table:table table:name=\"{@name}\">\n"
    "<xsl:apply-templates select=\"row\"/>\n"
  "</table:table>\n"
"</xsl:template>\n"

"<xsl:template match=\"row\">\n"
  "<table:table-row>\n"
    "<xsl:for-each select=\"cell\">\n"
    "<xsl:choose>\n"
      "<xsl:when test=\"@covered='true'\">\n"
        "<table:covered-table-cell/>\n"
      "</xsl:when>\n"
      "<xsl:when test=\"@type='object'\">\n"
        "<table:table-cell>\n"
          "<xsl:apply-templates select=\".\"/>\n"
        "</table:table-cell>\n"
      "</xsl:when>\n"
      "<xsl:when test=\"@type='image'\">\n"
        "<table:table-cell>\n"
          "<xsl:apply-templates select=\".\"/>\n"
        "</table:table-cell>\n"
      "</xsl:when>\n"
      "<xsl:otherwise>\n"
        "<table:table-cell office:value-type=\"{@type}\" \n"
                          "office:value=\"{.}\">\n"
         "<xsl:if test=\"@style or @bgcolor or @fgcolor\">\n"
          "<xsl:attribute name=\"table:style-name\">\n"
            "<xsl:text>cell-style-</xsl:text>\n"
            "<xsl:value-of select=\"@style\"/>\n"
            "<xsl:text>BG</xsl:text>\n"
            "<xsl:value-of select=\"@bgcolor\"/>\n"
            "<xsl:text>FG</xsl:text>\n"
            "<xsl:value-of select=\"@fgcolor\"/>\n"
          "</xsl:attribute>\n"
        "</xsl:if>\n"
          "<xsl:if test=\"@column-span > 0 or @row-span > 0\">\n"
            "<xsl:attribute name=\"table:number-columns-spanned\">\n"
              "<xsl:choose>\n"
                "<xsl:when test=\"@column-span > 0\">\n"
                  "<xsl:value-of select=\"@column-span\"/>\n"
                "</xsl:when>\n"
                "<xsl:otherwise>\n"
                  "<xsl:text>1</xsl:text>\n"
                "</xsl:otherwise>\n"
              "</xsl:choose>\n"
            "</xsl:attribute>\n"
            "<xsl:attribute name=\"table:number-rows-spanned\">\n"
              "<xsl:choose>\n"
                "<xsl:when test=\"@row-span > 0\">\n"
                  "<xsl:value-of select=\"@row-span\"/>\n"
                "</xsl:when>\n"
                "<xsl:otherwise>\n"
                  "<xsl:text>1</xsl:text>\n"
                "</xsl:otherwise>\n"
              "</xsl:choose>\n"
            "</xsl:attribute>\n"
          "</xsl:if>\n"
          "<text:p>\n"
            "<xsl:value-of select=\".\"/>\n"
          "</text:p>\n"
        "</table:table-cell>\n"
        "<xsl:if test=\"@column-span > 0\">\n"
          "<xsl:call-template name=\"generate-covered-cells\">\n"
            "<xsl:with-param name=\"count\" select=\"@column-span - 1\"/>\n"
          "</xsl:call-template>\n"
        "</xsl:if>\n"
      "</xsl:otherwise>\n"
    "</xsl:choose>\n"
    "</xsl:for-each>\n"
  "</table:table-row>\n"
"</xsl:template>\n"

"<xsl:template name=\"generate-covered-cells\">\n"
  "<xsl:param name=\"count\"/>\n"
  "<xsl:if test=\"$count > 0\">\n"
    "<!-- This can be done using \"table:number-columns-repeated\" too.. -->\n"
    "<table:covered-table-cell/>\n"
    "<xsl:call-template name=\"generate-covered-cells\">\n"
      "<xsl:with-param name=\"count\" select=\"$count - 1\"/>\n"
    "</xsl:call-template>\n"
  "</xsl:if>\n"
"</xsl:template>\n"

"<xsl:template match=\"chart\">\n"
  "<draw:frame svg:width=\"{@width}\" \n"
              "svg:height=\"{@height}\">\n"
    "<draw:object xlink:href=\"./{@name}\" \n"
                 "xlink:type=\"simple\" \n"
                 "xlink:show=\"embed\" \n"
                 "xlink:actuate=\"onLoad\"/>\n"
  "</draw:frame>\n"
"</xsl:template>\n"

"<xsl:template match=\"image\">\n"
  "<draw:frame draw:name=\"{@name}\"\n"
              "svg:width=\"{@width}\" \n"
              "svg:height=\"{@height}\"\n"
              "svg:x=\"{@x}\"\n"
              "svg:y=\"{@y}\">\n"
    "<draw:image  xlink:href=\"Pictures/{@name}.png\" \n"
                 "xlink:type=\"simple\" \n"
                 "xlink:show=\"embed\" \n"
                 "xlink:actuate=\"onLoad\"/>\n"
  "</draw:frame>\n"
"</xsl:template>\n"

"</xsl:stylesheet>\n");





QString data_ext_charts("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<xsl:stylesheet \n"
  "version=\"1.0\"\n"
  "xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\"\n"
  "xmlns=\"http://www.w3.org/1999/xhtml\">\n"
"<xsl:output method=\"xml\" encoding=\"UTF-8\"/>\n"
"<xsl:template match=\"/spreadsheet\">\n"

"<xsl:apply-templates select=\"sheet\"/>\n"
"</xsl:template>\n"

"<xsl:template match=\"sheet\">\n"
  "<xsl:apply-templates select=\"row\"/>\n"
"</xsl:template>\n"

"<xsl:template match=\"row\">\n"
    "<xsl:for-each select=\"cell\">\n"
    "<xsl:choose>\n"
      "<xsl:when test=\"@type='object'\">\n"
          "<xsl:apply-templates select=\"./*\"/>\n"
          "<xsl:text>__SEP__</xsl:text>\n"
      "</xsl:when>\n"
    "</xsl:choose>\n"
    "</xsl:for-each>\n"
"</xsl:template>\n"

"<xsl:template match=\"@*|node()\">\n"
  "<xsl:copy>\n"
    "<xsl:apply-templates select=\"@*|node()\"/>\n"
  "</xsl:copy>\n"
"</xsl:template>\n"

"</xsl:stylesheet>\n");




QString data_ext_imgs("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<xsl:stylesheet \n" 
  "version=\"1.0\"\n"
  "xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\"\n"
  "xmlns=\"http://www.w3.org/1999/xhtml\">\n"
"<xsl:output method=\"xml\" encoding=\"UTF-8\"/>\n"
"<xsl:template match=\"/spreadsheet\">\n"

"<xsl:apply-templates select=\"sheet\"/>\n"
"</xsl:template>\n"

"<xsl:template match=\"sheet\">\n"
  "<xsl:apply-templates select=\"row\"/>\n"
"</xsl:template>\n"

"<xsl:template match=\"row\">\n"
    "<xsl:for-each select=\"cell\">\n"
    "<xsl:choose>\n"
      "<xsl:when test=\"@type='image'\">\n"
          "<xsl:apply-templates select=\"./*\"/>\n"
          "<xsl:text>__SEP__</xsl:text>\n"
      "</xsl:when>\n"
    "</xsl:choose>\n"
    "</xsl:for-each>\n"
"</xsl:template>\n"

"<xsl:template match=\"@*|node()\">\n"
  "<xsl:copy>\n"
    "<xsl:apply-templates select=\"@*|node()\"/>\n"
  "</xsl:copy>\n"
"</xsl:template>\n"

"</xsl:stylesheet>\n");


QString data_chart("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<xsl:stylesheet \n"
  "version=\"1.0\" \n"
  "xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\" \n"
  "xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" \n" 
  "xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\" \n" 
  "xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" \n" 
  "xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" \n" 
  "xmlns:draw=\"urn:oasis:names:tc:opendocument:xmlns:drawing:1.0\" \n" 
  "xmlns:fo=\"urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0\" \n"
  "xmlns:xlink=\"http://www.w3.org/1999/xlink\" \n"
  "xmlns:dc=\"http://purl.org/dc/elements/1.1/\" \n"
  "xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" \n"
  "xmlns:number=\"urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0\" \n"
  "xmlns:presentation=\"urn:oasis:names:tc:opendocument:xmlns:presentation:1.0\" \n"
  "xmlns:svg=\"urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0\" \n"
  "xmlns:chart=\"urn:oasis:names:tc:opendocument:xmlns:chart:1.0\" \n"
  "xmlns:dr3d=\"urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0\" \n"
  "xmlns:math=\"http://www.w3.org/1998/Math/MathML\" \n"
  "xmlns:form=\"urn:oasis:names:tc:opendocument:xmlns:form:1.0\" \n"
  "xmlns:script=\"urn:oasis:names:tc:opendocument:xmlns:script:1.0\" \n"
  "xmlns:ooo=\"http://openoffice.org/2004/office\" \n"
  "xmlns:ooow=\"http://openoffice.org/2004/writer\" \n"
  "xmlns:oooc=\"http://openoffice.org/2004/calc\" \n"
  "xmlns:dom=\"http://www.w3.org/2001/xml-events\" \n"
  "xmlns:xforms=\"http://www.w3.org/2002/xforms\" \n"
  "xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" \n"
  "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" \n"
  "xmlns:rpt=\"http://openoffice.org/2005/report\" \n"
  "xmlns:of=\"urn:oasis:names:tc:opendocument:xmlns:of:1.2\" \n"
  "xmlns:rdfa=\"http://docs.oasis-open.org/opendocument/meta/rdfa#\"\n"
  "xmlns:field=\"urn:openoffice:names:experimental:ooxml-odf-interop:xmlns:field:1.0\" \n"
  "xmlns:formx=\"urn:openoffice:names:experimental:ooxml-odf-interop:xmlns:form:1.0\" \n"
  "xmlns=\"http://www.w3.org/1999/xhtml\">\n"
"<xsl:output method=\"xml\" indent=\"yes\" encoding=\"UTF-8\"/>\n"
"<xsl:template match=\"/chart\">\n"
"<office:document-content \n"
  "xmlns:office=\"urn:oasis:names:tc:opendocument:xmlns:office:1.0\" \n"
  "xmlns:style=\"urn:oasis:names:tc:opendocument:xmlns:style:1.0\" \n"
  "xmlns:text=\"urn:oasis:names:tc:opendocument:xmlns:text:1.0\" \n"
  "xmlns:table=\"urn:oasis:names:tc:opendocument:xmlns:table:1.0\" \n"
  "xmlns:draw=\"urn:oasis:names:tc:opendocument:xmlns:drawing:1.0\" \n"
  "xmlns:fo=\"urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0\" \n"
  "xmlns:xlink=\"http://www.w3.org/1999/xlink\" \n"
  "xmlns:dc=\"http://purl.org/dc/elements/1.1/\" \n"
  "xmlns:meta=\"urn:oasis:names:tc:opendocument:xmlns:meta:1.0\" \n"
  "xmlns:number=\"urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0\" \n"
  "xmlns:presentation=\"urn:oasis:names:tc:opendocument:xmlns:presentation:1.0\"\n"
  "xmlns:svg=\"urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0\" \n"
  "xmlns:chart=\"urn:oasis:names:tc:opendocument:xmlns:chart:1.0\" \n"
  "xmlns:dr3d=\"urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0\" \n"
  "xmlns:math=\"http://www.w3.org/1998/Math/MathML\" \n"
  "xmlns:form=\"urn:oasis:names:tc:opendocument:xmlns:form:1.0\" \n"
  "xmlns:script=\"urn:oasis:names:tc:opendocument:xmlns:script:1.0\" \n"
  "xmlns:ooo=\"http://openoffice.org/2004/office\" \n"
  "xmlns:ooow=\"http://openoffice.org/2004/writer\" \n"
  "xmlns:oooc=\"http://openoffice.org/2004/calc\" \n"
  "xmlns:dom=\"http://www.w3.org/2001/xml-events\" \n"
  "xmlns:xforms=\"http://www.w3.org/2002/xforms\" \n"
  "xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" \n"
  "xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" \n"
  "xmlns:rpt=\"http://openoffice.org/2005/report\" \n"
  "xmlns:of=\"urn:oasis:names:tc:opendocument:xmlns:of:1.2\" \n"
  "xmlns:rdfa=\"http://docs.oasis-open.org/opendocument/meta/rdfa#\"\n"
  "xmlns:field=\"urn:openoffice:names:experimental:ooxml-odf-interop:xmlns:field:1.0\" \n"
  "xmlns:formx=\"urn:openoffice:names:experimental:ooxml-odf-interop:xmlns:form:1.0\" \n"
  "office:version=\"1.2\">\n"

  "<office:automatic-styles>\n"
    "<number:number-style style:name=\"N0\">\n"
      "<number:number number:min-integer-digits=\"1\"/>\n"
    "</number:number-style>\n"
    "<style:style style:name=\"chart-style\" \n"
                 "style:family=\"chart\">\n"
      "<style:graphic-properties draw:stroke=\"none\"/>\n"
    "</style:style>\n"
    "<style:style style:name=\"chart-title-style\" \n"
                 "style:family=\"chart\">\n"
      "<style:text-properties fo:font-size=\"13pt\" \n"
                             "style:font-size-asian=\"13pt\" \n"
                             "style:font-size-complex=\"13pt\"/>\n"
    "</style:style>\n"
    "<style:style style:name=\"chart-subtitle-style\" \n"
                 "style:family=\"chart\">\n"
      "<style:text-properties fo:font-size=\"11pt\" \n"
                             "style:font-size-asian=\"11pt\" \n"
                             "style:font-size-complex=\"11pt\"/>\n"
    "</style:style>\n"
    "<style:style style:name=\"legend-style\" \n"
                 "style:family=\"chart\">\n"
      "<style:graphic-properties draw:stroke=\"none\" \n"
                                "svg:stroke-color=\"#b3b3b3\" \n"
                                "draw:fill=\"none\" \n"
                                "draw:fill-color=\"#e6e6e6\"/>\n"
      "<style:text-properties fo:font-size=\"8pt\" \n"
                             "style:font-size-asian=\"8pt\" \n"
                             "style:font-size-complex=\"8pt\"/>\n"
    "</style:style>\n"
    "<style:style style:name=\"plot-area-style\" \n"
                 "style:family=\"chart\">\n"
      "<style:chart-properties chart:symbol-type=\"automatic\" \n"
                              "chart:spline-order=\"3\" \n"
                              "chart:sort-by-x-values=\"false\" \n"
                              "chart:right-angled-axes=\"true\"/>\n"
    "</style:style>\n"
    "<style:style style:name=\"axis-style\" \n"
                 "style:family=\"chart\" \n"
                 "style:data-style-name=\"N0\">\n"
      "<style:chart-properties chart:display-label=\"true\" \n"
                              "chart:logarithmic=\"false\" \n"
                              "chart:reverse-direction=\"false\" \n"
                              "text:line-break=\"false\"/>\n"
      "<style:graphic-properties svg:stroke-color=\"#b3b3b3\"/>\n"
      "<style:text-properties fo:font-size=\"8pt\" \n"
                             "style:font-size-asian=\"8pt\" \n"
                             "style:font-size-complex=\"8pt\"/>\n"
    "</style:style>\n"
    "<style:style style:name=\"axis-title-style\" \n"
                 "style:family=\"chart\">\n"
      "<style:text-properties fo:font-size=\"9pt\" \n"
                             "style:font-size-asian=\"9pt\" \n"
                             "style:font-size-complex=\"9pt\"/>\n"
    "</style:style>\n"
    "<style:style style:name=\"grid-style\" \n"
                 "style:family=\"chart\">\n"
      "<style:graphic-properties svg:stroke-color=\"#b3b3b3\"/>\n"
    "</style:style>\n"
    "<style:style style:name=\"wall-style\" \n"
                 "style:family=\"chart\">\n"
      "<style:graphic-properties draw:stroke=\"solid\" \n"
                                "svg:stroke-color=\"#b3b3b3\" \n"
                                "draw:fill=\"none\" \n"
                                "draw:fill-color=\"#e6e6e6\"/>\n"
    "</style:style>\n"
    "<style:style style:name=\"floor-style\" \n"
                 "style:family=\"chart\">\n"
      "<style:graphic-properties draw:stroke=\"none\" \n"
                                "svg:stroke-color=\"#b3b3b3\" \n"
                                "draw:fill=\"solid\" \n"
                                "draw:fill-color=\"#cccccc\"/>\n"
    "</style:style>\n"
    "<xsl:for-each select=\"series\">\n"
      "<style:style style:name=\"{@name-address}-style\" \n"
                   "style:family=\"chart\">\n"
        "<style:chart-properties chart:symbol-type=\"named-symbol\" \n"
                                "chart:symbol-name=\"square\" \n"
                                "chart:symbol-width=\"0.1cm\" \n"
                                "chart:symbol-height=\"0.1cm\"/>\n"
        "<style:graphic-properties draw:stroke=\"solid\" \n"
                                  "svg:stroke-width=\"0.05cm\" \n"
                                  "svg:stroke-color=\"{@color}\" \n"
                                  "draw:fill-color=\"{@color}\" \n"
                                  "dr3d:edge-rounding=\"0%\"/>\n"
        "<style:text-properties fo:font-size=\"6pt\" \n"
                               "style:font-size-asian=\"6pt\" \n"
                               "style:font-size-complex=\"6pt\"/>\n"
      "</style:style>\n"
    "</xsl:for-each>\n"
  "</office:automatic-styles>\n"

"<office:body>\n"
"<office:chart>\n"
  "<chart:chart svg:width=\"{@width}\" \n"
               "svg:height=\"{@height}\" \n"
               "chart:class=\"chart:scatter\"\n"
               "chart:style-name=\"chart-style\">\n"
    "<xsl:if test=\"not(string-length(@title)=0)\">\n"
      "<chart:title chart:style-name=\"chart-title-style\">\n"
        "<text:p><xsl:value-of select=\"@title\"/></text:p>\n"
      "</chart:title>\n"
    "</xsl:if>\n"
    "<xsl:if test=\"not(string-length(@subtitle)=0)\">\n"
      "<chart:subtitle chart:style-name=\"chart-subtitle-style\">\n"
        "<text:p><xsl:value-of select=\"@subtitle\"/></text:p>\n"
      "</chart:subtitle>\n"
    "</xsl:if>\n"
    "<chart:legend chart:legend-position=\"end\" \n"
                  "chart:style-name=\"legend-style\"/>\n"
    "<chart:plot-area chart:style-name=\"plot-area-style\" \n"
                     "table:cell-range-address=\"{@range}\"\n"
                     "chart:data-source-has-labels=\"row\">\n"
      "<chart:axis chart:dimension=\"x\" \n"
                  "chart:name=\"primary-x\"\n"
                  "chart:style-name=\"axis-style\">\n"
        "<chart:title chart:style-name=\"axis-title-style\">\n"
          "<text:p><xsl:value-of select=\"@x-axis-label\"/></text:p>\n"
        "</chart:title>\n"
      "</chart:axis>\n"
      "<chart:axis chart:dimension=\"y\" \n"
                  "chart:name=\"primary-y\"\n"
                  "chart:style-name=\"axis-style\">\n"
        "<chart:grid chart:style-name=\"grid-style\" \n"
                    "chart:class=\"major\"/>\n"
        "<chart:title chart:style-name=\"axis-title-style\">\n"
          "<text:p><xsl:value-of select=\"@y-axis-label\"/></text:p>\n"
        "</chart:title>\n"
      "</chart:axis>\n"
      "<xsl:apply-templates select=\"series\"/>\n"
      "<chart:wall chart:style-name=\"wall-style\"/>\n"
      "<chart:floor chart:style-name=\"floor-style\"/>\n"
    "</chart:plot-area>\n"
  "</chart:chart>\n"
"</office:chart>\n"
"</office:body>\n"
"</office:document-content>\n"
"</xsl:template>\n"

"<xsl:template match=\"series\">\n"
  "<chart:series chart:style-name=\"{@name-address}-style\"\n"
                "chart:values-cell-range-address=\"{@y-range}\"\n"
                "chart:label-cell-address=\"{@name-address}\"\n"
                "chart:class=\"chart:scatter\">\n"
    "<chart:domain table:cell-range-address=\"{@x-range}\"/>\n"
  "</chart:series>\n"
"</xsl:template>\n"

"</xsl:stylesheet>\n");



bool isXmlCharODS(const QChar &c)
{
  // Characters in this range must be accepted by XML parsers.
  // Consequently characters outside of this range need to be escaped.

  ushort uc = c.unicode();

  return uc == 0x9
         || uc == 0xA
         || uc == 0xD
         || 0x20 <= uc && uc <= 0xD7FF
         || 0xE000 <= uc && uc <= 0xFFFD;
}

// ### AbanQ
// to do compliant with the standar XML 1.0
// 2.11 End-of-Line Handling and 3.3.3 Attribute-Value Normalization
// see below encodeAttrODS
bool isEndOfLineCharODS(const QChar &c)
{
  ushort uc = c.unicode();

  return uc == 0x9
         || uc == 0xA
         || uc == 0xD;
}
// ### AbanQ

QString encodeAttrODS(const QString &str)
{
  QString tmp(str);
  uint len = tmp.length();
  uint i = 0;
  while (i < len) {
    if (tmp[(int)i] == '<') {
      tmp.replace(i, 1, "&lt;");
      len += 3;
      i += 4;
    } else if (tmp[(int)i] == '"') {
      tmp.replace(i, 1, "&quot;");
      len += 5;
      i += 6;
    } else if (tmp[(int)i] == '&') {
      tmp.replace(i, 1, "&amp;");
      len += 4;
      i += 5;
    } else if (tmp[(int)i] == '>' && i >= 2 && tmp[(int)i - 1] == ']' && tmp[(int)i - 2] == ']') {
      tmp.replace(i, 1, "&gt;");
      len += 3;
      i += 4;
    }
    // ### AbanQ
    // to do compliant with the standar XML 1.0
    // 2.11 End-of-Line Handling and 3.3.3 Attribute-Value Normalization
    else if (isEndOfLineCharODS(tmp[(int)i])) {
      QString repl = "&#x" + QString::number(tmp[(int)i].unicode(), 16) + ';';
      tmp.replace(i, 1, repl);
      len += repl.length() - 1;
      i += repl.length();
    }
    // ### AbanQ
    else if (!isXmlCharODS(tmp[(int)i])) {
      QString repl = "&#x" + QString::number(tmp[(int)i].unicode(), 16) + ';';
      qWarning("AQOdsRow: not saving invalid character %s, the document will not be well-formed", repl.latin1());
      repl = "?";
      tmp.replace(i, 1, repl);
      len += repl.length() - 1;
      i += repl.length();
    } else {
      ++i;
    }
  }

  return tmp;
}





static inline QString chartName(const QString &chart)
{
  QString str("<chart name=\"");
  int start = chart.find(str);
  if (start < 0)
    return QString::null;
  int pos = start + str.length();
  int end = chart.find("\"", pos);
  return chart.mid(pos, end - pos);
}

static inline QString imageName(const QString &image)
{
  QString str("<image name=\"");
  int start = image.find(str);
  if (start < 0)
    return QString::null;
  int pos = start + str.length();
  int end = image.find("\"", pos);
  return image.mid(pos, end - pos);
}

static inline QString imageLink(const QString &image)
{
  QString str(" link=\"");
  int start = image.find(str);
  if (start < 0)
    return QString::null;
  int pos = start + str.length();
  int end = image.find("\"", pos);
  return image.mid(pos, end - pos);
}

AQOdsGenerator::AQOdsGenerator()
{
  clear();
}

AQOdsGenerator::~AQOdsGenerator()
{
  if (!fileName_.isEmpty()) {
    if (fileStream_.is_open()) {
      fileStream_.flush();
      fileStream_.close();
    }
    QFile::remove(fileName_);
  }
}

void AQOdsGenerator::clear()
{
  if (!fileName_.isEmpty()) {
    if (fileStream_.is_open()) {
      fileStream_.flush();
      fileStream_.close();
    }
    QFile::remove(fileName_);
  }
  strStream_.str("");
  fileName_ = AQ_DISKCACHE_DIRPATH + QString::fromLatin1("/report_") +
              QDateTime::currentDateTime().toString("ddMMyyyyhhmmsszzz") +
              QString::fromLatin1(".str");
  fileStream_.open(fileName_, std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
  if (!fileStream_.is_open())
    qWarning("AQOdsGenerator::clear : Error opening " + fileName_);
}

QString AQOdsGenerator::str()
{
  if (fileStream_.is_open()) {
    fileStream_.flush();
    fileStream_.close();
    fileStream_.open(fileName_, std::ios_base::in | std::ios_base::out | std::ios_base::app);
    strStream_ << fileStream_.rdbuf();
    fileStream_.seekp(0, std::ios_base::end);
  }
  return strStream_.str();
}

bool AQOdsGenerator::generateOds(const QString &fileNameOut)
{
  AQS *aqs = globalAQS;
  QByteArray baMain(data_main.utf8());
  QByteArray baChart(data_chart.utf8());
  QByteArray baExtCharts(data_ext_charts.utf8());
  QByteArray baExtImgs(data_ext_imgs.utf8());
  

  QString strInput(str());
  QByteArray input(strInput.utf8());
  QByteArray content(aqs->xsltproc(&baMain, &input));
  // ------------ vvvv   AbanQ 2.5    vvvv  ------
  QString fileNameContent(fileName_ + QString::fromLatin1(".cnt"));
  aqs->xsltproc(&baMain, fileName_, fileNameContent);

  QString errMsg;
  int errLine, errColumn;
  QFile filAux(fileNameContent);
  filAux.open(IO_ReadOnly);
  QDomDocument docTmp;
  if (!docTmp.setContent(&filAux, &errMsg, &errLine, &errColumn)) {
    qWarning("'%s': XML error %s  line: %d  column: %d",
             fileNameContent.latin1(), errMsg.latin1(),
             errLine, errColumn);
  } else {
    QString cntAux(docTmp.toString());
    cntAux.replace("__HREF1__", "<text:a xlink:href");
    cntAux.replace("__HREF2__", ">");
    cntAux.replace("__HREF3__", "</text:a>");
    filAux.close();

    if (!docTmp.setContent(cntAux, &errMsg, &errLine, &errColumn)) {
      qWarning("'%s cntAux': XML error %s  line: %d  column: %d",
               fileNameContent.latin1(), errMsg.latin1(),
               errLine, errColumn);
    } else {
      QFile::remove(fileNameContent);
      filAux.open(IO_WriteOnly);
      QTextStream tsAux(&filAux);
      docTmp.save(tsAux, 0);
      filAux.close();
    }
  }
  // ----------- ^^^^^                ^^^^^^
  QStringList charts;
  if (strInput.contains("<chart name=\"")) {
    charts = QStringList::split("__SEP__",
                                aqs->xsltproc(&baExtCharts, fileName_));
  }

  QValueList<QPair<QString, QByteArray> > objects;
  for (QStringList::const_iterator it = charts.begin(); it != charts.end(); ++it) {
    QCString chart(*it);
    if (chart.isEmpty() || chart == "\n")
      continue;
    objects.append(
      qMakePair(chartName(chart),
                aqs->xsltproc(&baChart, &chart))
    );
  }

  QStringList images;
  if (strInput.contains("<image name=\"")) {
    images = QStringList::split("__SEP__",
                                aqs->xsltproc(&baExtImgs, fileName_));
  }

  QValueList<QPair<QString, QString> > imgs;
  for (QStringList::const_iterator it = images.begin(); it != images.end(); ++it) {
    QCString image(*it);
    if (image.isEmpty() || image == "\n")
      continue;
    imgs.append(
      qMakePair(imageLink(image),
                imageName(image))
    );
  }

  AQZipWriter zip(fileNameOut);

  QByteArray mimeType(QCString("application/vnd.oasis.opendocument.spreadsheet"));
  mimeType.truncate(mimeType.size() - 1);
  zip.addFile("mimetype", mimeType);
  QFile filCont(fileNameContent);
  zip.addFile("content.xml", &filCont);

  for (int i = 0; i < objects.size(); ++i) {
    QPair<QString, QByteArray> object = objects[i];
    QString path(object.first + "/content.xml");
    zip.addFile(path, object.second);
  }

  for (int i = 0; i < imgs.size(); ++i) {
    QPair<QString, QString> img = imgs[i];
    QString path("Pictures/" + img.second + ".png");
    QString link(img.first);
    QFile fil(link);
    if (!fil.open(IO_ReadOnly))
      continue;
    zip.addFile(path, fil.readAll());
  }

  QString manifest("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
                   "<manifest:manifest"
                   " xmlns:manifest=\"urn:oasis:names:tc:opendocument:xmlns:manifest:1.0\">\n"
                   " <manifest:file-entry"
                   " manifest:media-type=\"application/vnd.oasis.opendocument.spreadsheet\""
                   " manifest:version=\"1.2\""
                   " manifest:full-path=\"/\"/>\n"
                   " <manifest:file-entry"
                   " manifest:media-type=\"text/xml\""
                   " manifest:full-path=\"content.xml\"/>\n");

  for (int i = 0; i < objects.size(); ++i) {
    QPair<QString, QByteArray> object = objects[i];
    manifest += QString(" <manifest:file-entry"
                        " manifest:media-type=\"text/xml\""
                        " manifest:full-path=\"%1/content.xml\"/>\n")
                .arg(object.first);

    manifest += QString(" <manifest:file-entry"
                        " manifest:media-type=\"application/vnd.oasis.opendocument.chart\""
                        " manifest:full-path=\"%1/\"/>\n")
                .arg(object.first);
  }

  if (!imgs.isEmpty()) {
    for (int i = 0; i < imgs.size(); ++i) {
      QPair<QString, QString> img = imgs[i];
      manifest += QString(" <manifest:file-entry"
                          " manifest:media-type=\"image/png\""
                          " manifest:full-path=\"Pictures/%1.png\"/>\n")
                  .arg(img.second);
    }

    manifest += QString(" <manifest:file-entry"
                        " manifest:media-type=\"\""
                        " manifest:full-path=\"Pictures/\"/>\n");
  }

  manifest += "</manifest:manifest>";
  QByteArray mf(manifest.utf8());
  mf.truncate(mf.size() - 1);
  zip.addFile("META-INF/manifest.xml", mf);
  zip.close();

  QFile::remove(fileNameContent);

  return true;
}
