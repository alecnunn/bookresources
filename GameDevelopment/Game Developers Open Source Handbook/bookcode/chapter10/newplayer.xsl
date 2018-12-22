<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
  <newlevel>
    <xsl:apply-templates select="map"/>
  </newlevel>
</xsl:template>


<xsl:template match="player">
 <player x="{start/position/@x}" y="{start/position/@y}" z="{start/position/@z}">
    <xsl:apply-templates/>
 </player>
</xsl:template>

<xsl:template match="health">
  <health value="{@value}"/>
</xsl:template>


<xsl:template match="ai">
 <ai x="{position/@x}" y="{position/@y}" z="{position/@z}"/>
</xsl:template>


</xsl:stylesheet>