<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet 
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0" 
  xmlns:style="urn:oasis:names:tc:opendocument:xmlns:style:1.0" 
  xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0" 
  xmlns:table="urn:oasis:names:tc:opendocument:xmlns:table:1.0" 
  xmlns:draw="urn:oasis:names:tc:opendocument:xmlns:drawing:1.0" 
  xmlns:fo="urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0" 
  xmlns:xlink="http://www.w3.org/1999/xlink" 
  xmlns:dc="http://purl.org/dc/elements/1.1/" 
  xmlns:meta="urn:oasis:names:tc:opendocument:xmlns:meta:1.0" 
  xmlns:number="urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0" 
  xmlns:presentation="urn:oasis:names:tc:opendocument:xmlns:presentation:1.0"
  xmlns:svg="urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0" 
  xmlns:chart="urn:oasis:names:tc:opendocument:xmlns:chart:1.0" 
  xmlns:dr3d="urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0" 
  xmlns:math="http://www.w3.org/1998/Math/MathML" 
  xmlns:form="urn:oasis:names:tc:opendocument:xmlns:form:1.0" 
  xmlns:script="urn:oasis:names:tc:opendocument:xmlns:script:1.0" 
  xmlns:ooo="http://openoffice.org/2004/office" 
  xmlns:ooow="http://openoffice.org/2004/writer" 
  xmlns:oooc="http://openoffice.org/2004/calc" 
  xmlns:dom="http://www.w3.org/2001/xml-events" 
  xmlns:xforms="http://www.w3.org/2002/xforms" 
  xmlns:xsd="http://www.w3.org/2001/XMLSchema" 
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
  xmlns:rpt="http://openoffice.org/2005/report" 
  xmlns:of="urn:oasis:names:tc:opendocument:xmlns:of:1.2" 
  xmlns:rdfa="http://docs.oasis-open.org/opendocument/meta/rdfa#"
  xmlns:field="urn:openoffice:names:experimental:ooxml-odf-interop:xmlns:field:1.0" 
  xmlns:formx="urn:openoffice:names:experimental:ooxml-odf-interop:xmlns:form:1.0" 
  xmlns="http://www.w3.org/1999/xhtml">
<!-- See "Extracting Unique Values with XSL" 
     (http://www.bernzilla.com/item.php?id=333) -->
<!-- xsl:key name="cell-styles" match="cell" use="@style"/ -->
<xsl:output method="xml" indent="yes" encoding="UTF-8"/>
<xsl:template match="/spreadsheet">
<office:document-content 
  xmlns:office="urn:oasis:names:tc:opendocument:xmlns:office:1.0" 
  xmlns:style="urn:oasis:names:tc:opendocument:xmlns:style:1.0" 
  xmlns:text="urn:oasis:names:tc:opendocument:xmlns:text:1.0" 
  xmlns:table="urn:oasis:names:tc:opendocument:xmlns:table:1.0" 
  xmlns:draw="urn:oasis:names:tc:opendocument:xmlns:drawing:1.0" 
  xmlns:fo="urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0" 
  xmlns:xlink="http://www.w3.org/1999/xlink" 
  xmlns:dc="http://purl.org/dc/elements/1.1/" 
  xmlns:meta="urn:oasis:names:tc:opendocument:xmlns:meta:1.0" 
  xmlns:number="urn:oasis:names:tc:opendocument:xmlns:datastyle:1.0" 
  xmlns:presentation="urn:oasis:names:tc:opendocument:xmlns:presentation:1.0"
  xmlns:svg="urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0" 
  xmlns:chart="urn:oasis:names:tc:opendocument:xmlns:chart:1.0" 
  xmlns:dr3d="urn:oasis:names:tc:opendocument:xmlns:dr3d:1.0" 
  xmlns:math="http://www.w3.org/1998/Math/MathML" 
  xmlns:form="urn:oasis:names:tc:opendocument:xmlns:form:1.0" 
  xmlns:script="urn:oasis:names:tc:opendocument:xmlns:script:1.0" 
  xmlns:ooo="http://openoffice.org/2004/office" 
  xmlns:ooow="http://openoffice.org/2004/writer" 
  xmlns:oooc="http://openoffice.org/2004/calc" 
  xmlns:dom="http://www.w3.org/2001/xml-events" 
  xmlns:xforms="http://www.w3.org/2002/xforms" 
  xmlns:xsd="http://www.w3.org/2001/XMLSchema" 
  xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" 
  xmlns:rpt="http://openoffice.org/2005/report" 
  xmlns:of="urn:oasis:names:tc:opendocument:xmlns:of:1.2" 
  xmlns:rdfa="http://docs.oasis-open.org/opendocument/meta/rdfa#"
  xmlns:field="urn:openoffice:names:experimental:ooxml-odf-interop:xmlns:field:1.0" 
  xmlns:formx="urn:openoffice:names:experimental:ooxml-odf-interop:xmlns:form:1.0" 
  office:version="1.2">
<office:automatic-styles>
  <xsl:for-each select="//cell[@style!='' or @bgcolor!='' or @fgcolor!='']">
  <!-- xsl:for-each select="//cell[generate-id() = generate-id(key('cell-styles',@style)[1])]" -->
    <style:style style:name="cell-style-{@style}BG{@bgcolor}FG{@fgcolor}" 
                 style:family="table-cell">
      <style:table-cell-properties>
        <xsl:if test="@bgcolor">
          <xsl:attribute name="fo:background-color">
            <xsl:value-of select="@bgcolor"/>
           </xsl:attribute>
        </xsl:if>
        <xsl:choose>
          <xsl:when test="contains(@style,'border-bottom')">
            <xsl:attribute name="fo:border-bottom">
              <xsl:text>0.0008in solid #000000</xsl:text>
            </xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
            <xsl:attribute name="fo:border-bottom">
              <xsl:text>none</xsl:text>
            </xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="contains(@style,'border-left')">
            <xsl:attribute name="fo:border-left">
              <xsl:text>0.0008in solid #000000</xsl:text>
            </xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
            <xsl:attribute name="fo:border-left">
              <xsl:text>none</xsl:text>
            </xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="contains(@style,'border-right')">
            <xsl:attribute name="fo:border-right">
              <xsl:text>0.0008in solid #000000</xsl:text>
            </xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
            <xsl:attribute name="fo:border-right">
              <xsl:text>none</xsl:text>
            </xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
        <xsl:choose>
          <xsl:when test="contains(@style,'border-top')">
            <xsl:attribute name="fo:border-top">
              <xsl:text>0.0008in solid #000000</xsl:text>
            </xsl:attribute>
          </xsl:when>
          <xsl:otherwise>
            <xsl:attribute name="fo:border-top">
              <xsl:text>none</xsl:text>
            </xsl:attribute>
          </xsl:otherwise>
        </xsl:choose>
      </style:table-cell-properties>
      <style:paragraph-properties>
        <xsl:attribute name="fo:text-align">
          <xsl:choose>
            <xsl:when test="contains(@style,'align-left')">
              <xsl:text>begin</xsl:text>
            </xsl:when>
            <xsl:when test="contains(@style,'align-center')">
              <xsl:text>center</xsl:text>
            </xsl:when>
            <xsl:when test="contains(@style,'align-right')">
              <xsl:text>end</xsl:text>
            </xsl:when>
          </xsl:choose>
        </xsl:attribute>
      </style:paragraph-properties>
      <style:text-properties>
        <xsl:if test="@fgcolor">
          <xsl:attribute name="fo:color">
            <xsl:value-of select="@fgcolor"/>
          </xsl:attribute>
        </xsl:if>
        <xsl:if test="contains(@style,'text-bold')">
          <xsl:attribute name="fo:font-weight">
            <xsl:text>bold</xsl:text>
          </xsl:attribute>
          <xsl:attribute name="style:font-weight-asian">
            <xsl:text>bold</xsl:text>
          </xsl:attribute>
          <xsl:attribute name="style:font-weight-complex">
            <xsl:text>bold</xsl:text>
          </xsl:attribute>
        </xsl:if>
        <xsl:if test="contains(@style,'text-italic')">
          <xsl:attribute name="fo:font-style">
            <xsl:text>italic</xsl:text>
          </xsl:attribute>
          <xsl:attribute name="style:font-style-asian">
            <xsl:text>italic</xsl:text>
          </xsl:attribute>
          <xsl:attribute name="style:font-style-complex">
            <xsl:text>italic</xsl:text>
          </xsl:attribute>
        </xsl:if>
        <xsl:if test="contains(@style,'text-underline')">
          <xsl:attribute name="style:text-underline-style">
            <xsl:text>solid</xsl:text>
          </xsl:attribute>
          <xsl:attribute name="style:text-underline-width">
            <xsl:text>auto</xsl:text>
          </xsl:attribute>
          <xsl:attribute name="style:text-underline-color">
            <xsl:text>font-color</xsl:text>
          </xsl:attribute>
        </xsl:if>
      </style:text-properties>
    </style:style>
  </xsl:for-each>
</office:automatic-styles>
<office:body>
<office:spreadsheet>
<xsl:apply-templates select="sheet"/>
</office:spreadsheet>
</office:body>
</office:document-content>
</xsl:template>

<xsl:template match="sheet">
  <table:table table:name="{@name}">
    <xsl:apply-templates select="row"/>
  </table:table>
</xsl:template>

<xsl:template match="row">
  <table:table-row>
    <xsl:for-each select="cell">
    <xsl:choose>
      <xsl:when test="@covered='true'">
        <table:covered-table-cell/>
      </xsl:when>
      <xsl:when test="@type='object'">
        <table:table-cell>
          <xsl:apply-templates select="."/>
        </table:table-cell>
      </xsl:when>
      <xsl:when test="@type='image'">
        <table:table-cell>
          <xsl:apply-templates select="."/>
        </table:table-cell>
      </xsl:when>
      <xsl:otherwise>
        <table:table-cell office:value-type="{@type}" 
                          office:value="{.}">
         <xsl:if test="@style or @bgcolor or @fgcolor">
          <xsl:attribute name="table:style-name">
            <xsl:text>cell-style-</xsl:text>
            <xsl:value-of select="@style"/>
            <xsl:text>BG</xsl:text>
            <xsl:value-of select="@bgcolor"/>
            <xsl:text>FG</xsl:text>
            <xsl:value-of select="@fgcolor"/>
          </xsl:attribute>
        </xsl:if>
          <xsl:if test="@column-span > 0 or @row-span > 0">
            <xsl:attribute name="table:number-columns-spanned">
              <xsl:choose>
                <xsl:when test="@column-span > 0">
                  <xsl:value-of select="@column-span"/>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:text>1</xsl:text>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:attribute>
            <xsl:attribute name="table:number-rows-spanned">
              <xsl:choose>
                <xsl:when test="@row-span > 0">
                  <xsl:value-of select="@row-span"/>
                </xsl:when>
                <xsl:otherwise>
                  <xsl:text>1</xsl:text>
                </xsl:otherwise>
              </xsl:choose>
            </xsl:attribute>
          </xsl:if>
          <text:p>
            <xsl:value-of select="."/>
          </text:p>
        </table:table-cell>
        <xsl:if test="@column-span > 0">
          <xsl:call-template name="generate-covered-cells">
            <xsl:with-param name="count" select="@column-span - 1"/>
          </xsl:call-template>
        </xsl:if>
      </xsl:otherwise>
    </xsl:choose>
    </xsl:for-each>
  </table:table-row>
</xsl:template>

<xsl:template name="generate-covered-cells">
  <xsl:param name="count"/>
  <xsl:if test="$count > 0">
    <!-- This can be done using "table:number-columns-repeated" too.. -->
    <table:covered-table-cell/>
    <xsl:call-template name="generate-covered-cells">
      <xsl:with-param name="count" select="$count - 1"/>
    </xsl:call-template>
  </xsl:if>
</xsl:template>

<xsl:template match="chart">
  <draw:frame svg:width="{@width}" 
              svg:height="{@height}">
    <draw:object xlink:href="./{@name}" 
                 xlink:type="simple" 
                 xlink:show="embed" 
                 xlink:actuate="onLoad"/>
  </draw:frame>
</xsl:template>

<xsl:template match="image">
  <draw:frame draw:name="{@name}"
              svg:width="{@width}" 
              svg:height="{@height}"
              svg:x="{@x}"
              svg:y="{@y}">
    <draw:image  xlink:href="Pictures/{@name}.png" 
                 xlink:type="simple" 
                 xlink:show="embed" 
                 xlink:actuate="onLoad"/>
  </draw:frame>
</xsl:template>

</xsl:stylesheet>
