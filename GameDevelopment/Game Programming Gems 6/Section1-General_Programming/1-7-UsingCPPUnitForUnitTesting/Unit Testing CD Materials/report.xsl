<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns="http://www.w3.org/TR/xhtml1/strict">

<xsl:template match="/">
<html>
<head>
<title>Test Report</title>
<style>
table				{ color:#222222; font-size:10pt; }
th					{ font-weight:normal; color:#FFFFFF; background-color:#888888; }
tr.check			{ background-color:#EEEEEE }
td.check			{ background-color:#EEEEEE }
h1					{ color:#111111; font-family:'Times New Roman'; border-style:solid; border-width:0px; border-bottom-width:3px; border-bottom-color:#444488; }
h2					{ color:#222222; font-family:'Times New Roman'; border-style:solid; border-width:0px; border-bottom-width:2px; border-bottom-color:#444488; }
h3					{ color:#333333; font-family:'Times New Roman'; border-style:solid; border-width:0px; border-bottom-width:1px; border-bottom-color:#444488; margin-bottom:8px; }
h4					{ color:#444444; font-family:'Times New Roman'; border-style:solid; border-width:0px; border-bottom-width:1px; border-bottom-color:#CCCCDD; margin-bottom:8px; }
h5					{ color:#555555; font-family:'Times New Roman'; border-style:solid; border-width:0px; border-bottom-width:1px; border-bottom-color:#EEEEFF; margin-bottom:8px; }
h6					{ color:#666666; font-family:'Times New Roman'; border-style:solid; border-width:0px; border-bottom-width:1px; border-bottom-color:#F8F8FF; margin-bottom:8px; }

span.good			{ color:#006666; font-weight:bold; }
span.critical		{ color:#880000; font-weight:bold; }
</style>
</head>
<body>
	<h1>Test Report</h1>
  	<xsl:apply-templates select="/TestRun/*"/>
</body>
</html>
</xsl:template>

<xsl:template match="FailedTests">
    <h2>FailedTests</h2>
    <xsl:choose>
	<xsl:when test="FailedTest">
		<table>
			<tr>
				<th>id</th>
				<th>Name</th>
				<th>FailureType</th>
				<th>Location</th>
				<th>Message</th>
			</tr>
		  	<xsl:apply-templates select="FailedTest"/>
		</table>
	</xsl:when>
	<xsl:otherwise>
	    <span class="good">No failed test.</span>
	</xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template match="FailedTest">
	<tr>
	<td align="right" valign='top'><xsl:value-of select="@id"/></td>
	<td valign='top'><xsl:apply-templates select="Name"/></td>
	<td valign='top'><xsl:apply-templates select="FailureType"/></td>
	<td valign='top'><xsl:apply-templates select="Location"/></td>
	<td valign='top'><pre><xsl:apply-templates select="Message"/></pre></td>
	</tr>
</xsl:template>
<xsl:template match="Name|FailureType|Message"><xsl:value-of select="."/></xsl:template>
<xsl:template match="Location">
	<xsl:if test=".">
		line #<xsl:value-of select="Line"/> in <xsl:value-of select="File"/>
	</xsl:if>
</xsl:template>
  
<xsl:template match="SucessfulTests">
    <h2>SucessfulTests</h2>
    <xsl:choose>
	<xsl:when test="Test">
	<table>
		<tr>
			<th>id</th>
			<th>Name</th>
		</tr>
	  	<xsl:apply-templates select="Test"/>
	</table>
	</xsl:when>
	<xsl:otherwise>
	    <span class="critical">No sucessful test.</span>
	</xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template match="Test">
	<tr>
		<td align="right"><xsl:value-of select="@id"/></td>
		<td><xsl:apply-templates select="Name"/></td>
	</tr>
</xsl:template>

<xsl:template match="Statistics">
    <h2>Statistics</h2>
	<table>
		<tr>
			<th>Status</th>
			<th>Number</th>
		</tr>

		<tr>
			<td>Tests</td>
			<td align="right"><xsl:value-of select="Tests"/></td>
		</tr>

		<tr>
			<td>FailuresTotal</td>
			<td align="right"><xsl:value-of select="FailuresTotal"/></td>
		</tr>

		<tr>
			<td>Errors</td>
			<td align="right"><xsl:value-of select="Errors"/></td>
		</tr>

		<tr>
			<td>Failures</td>
			<td align="right"><xsl:value-of select="Failures"/></td>
		</tr>
		
	</table>
</xsl:template>

</xsl:stylesheet>
