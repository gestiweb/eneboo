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
<xsl:output method="xml" indent="yes" encoding="UTF-8"/>
<xsl:template match="/chart">
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
    <number:number-style style:name="N0">
      <number:number number:min-integer-digits="1"/>
    </number:number-style>
    <style:style style:name="chart-style" 
                 style:family="chart">
      <style:graphic-properties draw:stroke="none"/>
    </style:style>
    <style:style style:name="chart-title-style" 
                 style:family="chart">
      <style:text-properties fo:font-size="13pt" 
                             style:font-size-asian="13pt" 
                             style:font-size-complex="13pt"/>
    </style:style>
    <style:style style:name="chart-subtitle-style" 
                 style:family="chart">
      <style:text-properties fo:font-size="11pt" 
                             style:font-size-asian="11pt" 
                             style:font-size-complex="11pt"/>
    </style:style>
    <style:style style:name="legend-style" 
                 style:family="chart">
      <style:graphic-properties draw:stroke="none" 
                                svg:stroke-color="#b3b3b3" 
                                draw:fill="none" 
                                draw:fill-color="#e6e6e6"/>
      <style:text-properties fo:font-size="8pt" 
                             style:font-size-asian="8pt" 
                             style:font-size-complex="8pt"/>
    </style:style>
    <style:style style:name="plot-area-style" 
                 style:family="chart">
      <style:chart-properties chart:symbol-type="automatic" 
                              chart:spline-order="3" 
                              chart:sort-by-x-values="false" 
                              chart:right-angled-axes="true"/>
    </style:style>
    <style:style style:name="axis-style" 
                 style:family="chart" 
                 style:data-style-name="N0">
      <style:chart-properties chart:display-label="true" 
                              chart:logarithmic="false" 
                              chart:reverse-direction="false" 
                              text:line-break="false"/>
      <style:graphic-properties svg:stroke-color="#b3b3b3"/>
      <style:text-properties fo:font-size="8pt" 
                             style:font-size-asian="8pt" 
                             style:font-size-complex="8pt"/>
    </style:style>
    <style:style style:name="axis-title-style" 
                 style:family="chart">
      <style:text-properties fo:font-size="9pt" 
                             style:font-size-asian="9pt" 
                             style:font-size-complex="9pt"/>
    </style:style>
    <style:style style:name="grid-style" 
                 style:family="chart">
      <style:graphic-properties svg:stroke-color="#b3b3b3"/>
    </style:style>
    <style:style style:name="wall-style" 
                 style:family="chart">
      <style:graphic-properties draw:stroke="solid" 
                                svg:stroke-color="#b3b3b3" 
                                draw:fill="none" 
                                draw:fill-color="#e6e6e6"/>
    </style:style>
    <style:style style:name="floor-style" 
                 style:family="chart">
      <style:graphic-properties draw:stroke="none" 
                                svg:stroke-color="#b3b3b3" 
                                draw:fill="solid" 
                                draw:fill-color="#cccccc"/>
    </style:style>
    <xsl:for-each select="series">
      <style:style style:name="{@name-address}-style" 
                   style:family="chart">
        <style:chart-properties chart:symbol-type="named-symbol" 
                                chart:symbol-name="square" 
                                chart:symbol-width="0.1cm" 
                                chart:symbol-height="0.1cm"/>
        <style:graphic-properties draw:stroke="solid" 
                                  svg:stroke-width="0.05cm" 
                                  svg:stroke-color="{@color}" 
                                  draw:fill-color="{@color}" 
                                  dr3d:edge-rounding="0%"/>
        <style:text-properties fo:font-size="6pt" 
                               style:font-size-asian="6pt" 
                               style:font-size-complex="6pt"/>
      </style:style>
    </xsl:for-each>
  </office:automatic-styles>

<office:body>
<office:chart>
  <chart:chart svg:width="{@width}" 
               svg:height="{@height}" 
               chart:class="chart:scatter"
               chart:style-name="chart-style">
    <xsl:if test="not(string-length(@title)=0)">
      <chart:title chart:style-name="chart-title-style">
        <text:p><xsl:value-of select="@title"/></text:p>
      </chart:title>
    </xsl:if>
    <xsl:if test="not(string-length(@subtitle)=0)">
      <chart:subtitle chart:style-name="chart-subtitle-style">
        <text:p><xsl:value-of select="@subtitle"/></text:p>
      </chart:subtitle>
    </xsl:if>
    <chart:legend chart:legend-position="end" 
                  chart:style-name="legend-style"/>
    <chart:plot-area chart:style-name="plot-area-style" 
                     table:cell-range-address="{@range}"
                     chart:data-source-has-labels="row">
      <chart:axis chart:dimension="x" 
                  chart:name="primary-x"
                  chart:style-name="axis-style">
        <chart:title chart:style-name="axis-title-style">
          <text:p><xsl:value-of select="@x-axis-label"/></text:p>
        </chart:title>
      </chart:axis>
      <chart:axis chart:dimension="y" 
                  chart:name="primary-y"
                  chart:style-name="axis-style">
        <chart:grid chart:style-name="grid-style" 
                    chart:class="major"/>
        <chart:title chart:style-name="axis-title-style">
          <text:p><xsl:value-of select="@y-axis-label"/></text:p>
        </chart:title>
      </chart:axis>
      <xsl:apply-templates select="series"/>
      <chart:wall chart:style-name="wall-style"/>
      <chart:floor chart:style-name="floor-style"/>
    </chart:plot-area>
  </chart:chart>
</office:chart>
</office:body>
</office:document-content>
</xsl:template>

<xsl:template match="series">
  <chart:series chart:style-name="{@name-address}-style"
                chart:values-cell-range-address="{@y-range}"
                chart:label-cell-address="{@name-address}"
                chart:class="chart:scatter">
    <chart:domain table:cell-range-address="{@x-range}"/>
  </chart:series>
</xsl:template>

</xsl:stylesheet>
