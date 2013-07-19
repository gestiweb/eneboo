<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet 
  version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns="http://www.w3.org/1999/xhtml">
<xsl:output method="xml" encoding="UTF-8"/>
<xsl:template match="/spreadsheet">

<xsl:apply-templates select="sheet"/>
</xsl:template>

<xsl:template match="sheet">
  <xsl:apply-templates select="row"/>
</xsl:template>

<xsl:template match="row">
    <xsl:for-each select="cell">
    <xsl:choose>
      <xsl:when test="@type='image'">
          <xsl:apply-templates select="./*"/>
          <xsl:text>__SEP__</xsl:text>
      </xsl:when>
    </xsl:choose>
    </xsl:for-each>
</xsl:template>

<xsl:template match="@*|node()">
  <xsl:copy>
    <xsl:apply-templates select="@*|node()"/>
  </xsl:copy>
</xsl:template>

</xsl:stylesheet>
