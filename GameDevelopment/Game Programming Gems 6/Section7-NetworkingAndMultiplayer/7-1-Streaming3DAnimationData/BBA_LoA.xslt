<?xml version="1.0" encoding="UTF-8"?>
<!-- Level of Articulation for Bone Based Animation -->
<xsl:stylesheet xmlns:gbsd="urn:mpeg:mpeg21:dia:schema:gBSD:2003" xmlns:mpeg7="urn:mpeg:mpeg7:schema:2001" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" version="1.0">
	<xsl:output method="xml" version="1.0" encoding="UTF-8"/>

	<xsl:param name="nBoneMax">35</xsl:param>

	<!-- Verify there are Bones Present -->
	<xsl:template name="checkBonesPresent">
		<xsl:if test="//gbsd:Parameter[@name=':MG4:NoOfBones']/@value==0">
			<xsl:message terminate="yes">
			Error No Bones are Present
			</xsl:message>
		</xsl:if>
	</xsl:template>
	
	<!-- Update Bone Count -->
	<xsl:template match="gbsd:Parameter[@name=':MG4:NoOfBones']/gbsd:Value">
		<xsl:copy>
			<xsl:apply-templates select="@*"/>
			<xsl:value-of select="$nBoneMax"/>
		</xsl:copy>
	</xsl:template>
	
	<!-- Remove all Bone Masks above a certain range -->
	<xsl:template match="gbsd:Parameter[@syntacticalLabel=':MG4:BoneMask']">
		<xsl:variable name="num"><xsl:value-of select="number(substring(substring-after(./@marker,'B'), 1, 1))"/></xsl:variable>
		<xsl:if test="$num &lt; $nBoneMax">
			<xsl:copy>
				<xsl:apply-templates select="@*|node()"/>
			</xsl:copy>
		</xsl:if>
	</xsl:template>		
	
	<xsl:template match="gbsd:gBSDUnit[@syntacticalLabel=':MG4:Bone']">
		<xsl:variable name="num"><xsl:value-of select="number(substring(substring-after(./@marker,'B'), 1, 1))"/></xsl:variable>
		<xsl:if test="$num &lt; $nBoneMax">
			<xsl:copy>
				<xsl:apply-templates select="@*|node()"/>
			</xsl:copy>
		</xsl:if>
	</xsl:template>		
</xsl:stylesheet>
