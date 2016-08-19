<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
				xmlns:xs="http://www.w3.org/2001/XMLSchema"
				xmlns:oai_dc="http://www.openarchives.org/OAI/2.0/oai_dc/"
				xmlns:dc="http://purl.org/dc/elements/1.1/"
				xmlns:srw_dc="info:srw/schema/1/dc-schema"
				exclude-result-prefixes="xs"
				version="1.0">
    
	<!-- 
		s. http://www.openarchives.org/OAI/2.0/oai_dc.xsd
	-->
    
	<xsl:param name="toc-separator"> -- </xsl:param>
	<xsl:output method="xml" indent="yes"/>
    
	<xsl:template match="/ROOT">
		<oai_dc:dc>
			<xsl:apply-templates />
			<dc:date></dc:date>
			<dc:identifier></dc:identifier>
		</oai_dc:dc>
	</xsl:template>
	
	<xsl:template match="date">
		<dc:date>
			<xsl:value-of select="."/>
		</dc:date>
	</xsl:template>
	
	<xsl:template match="record/title">
		<dc:title>
			<xsl:value-of select='.'/>
		</dc:title>
	</xsl:template>
	
	<xsl:template match="fmt">
		<dc:format>
			<xsl:value-of select="."/>
		</dc:format>
	</xsl:template>
	
	<xsl:template match="type">
		<dc:type>
			<xsl:value-of select="."/>
		</dc:type>
	</xsl:template>
	
	<xsl:template match="subject">
		<dc:subject>
			<xsl:value-of select='.'/>
		</dc:subject>
	</xsl:template>
	
	<xsl:template match="description">
		<dc:description>
			<xsl:value-of select="."/>
		</dc:description>
	</xsl:template>
	
	<xsl:template match="interviewee | interviewer">
		<dc:contributor>
			<xsl:value-of select='.'/>
		</dc:contributor>
	</xsl:template>
	
	<xsl:template match="rights | usage">
		<dc:rights>
			<xsl:value-of select='.'/>
		</dc:rights>
	</xsl:template>

	<xsl:template match="*">
		<xsl:apply-templates />
	</xsl:template>
    
	<xsl:template match="text()" />
	
</xsl:stylesheet>