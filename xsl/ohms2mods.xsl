<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
				xmlns:xs="http://www.w3.org/2001/XMLSchema"
				xmlns:mods="http://www.loc.gov/mods/v3"
				exclude-result-prefixes="xs mods"
				version="1.0">
    
	<!-- 
		s. http://www.loc.gov/standards/mods/v3/mods-userguide-elements.html
	-->
    
	<xsl:param name="toc-separator"> -- </xsl:param>
	<xsl:output method="xml" indent="yes"/>
    
	<xsl:template match="/ROOT">
		<mods:mods
			xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
			xsi:schemaLocation="http://www.loc.gov/standards/mods/v3/mods-3-6.xsd">			
			<xsl:apply-templates select="record"/>
		</mods:mods>
	</xsl:template>
    
	<xsl:template match="record">
		<xsl:apply-templates />
		<mods:genre>oral history</mods:genre>
		<mods:originInfo>
			<mods:dateCaptured>
				<xsl:value-of select="date/text()"/>
			</mods:dateCaptured>
		</mods:originInfo>
		<mods:physicalDescription>
			<mods:digitalOrigin>born digital</mods:digitalOrigin>
			<mods:extent>
				<xsl:value-of select="duration"/>
			</mods:extent>
		</mods:physicalDescription>
		<mods:abstract type='description'>
			<xsl:value-of select="description"/>
		</mods:abstract>
		<mods:subject>
			<xsl:apply-templates select="subject" mode="subjects" />
		</mods:subject>
	</xsl:template>	
    
	<xsl:template match="record/title">
		<mods:titleInfo>
			<mods:title>
				<xsl:value-of select="." />
			</mods:title>
		</mods:titleInfo>
	</xsl:template>
    
	<xsl:template match="mediafile">
		<mods:typeOfResource>
			<xsl:choose>
				<xsl:when test="clip_format/text() = 'video'">moving image</xsl:when>
				<xsl:otherwise>sound recording</xsl:otherwise>
			</xsl:choose>
		</mods:typeOfResource>
	</xsl:template>
    
	<xsl:template match="interviewee | interviewer">
		<mods:name type="personal">
			<mods:namePart>
				<xsl:value-of select="." />
			</mods:namePart>
			<mods:role>
				<mods:roleTerm type="text">
					<xsl:value-of select="local-name()"/>
				</mods:roleTerm>
			</mods:role>
		</mods:name>
	</xsl:template>
    
	<xsl:template match="index">
		<mods:relatedItem type="constituent">
			<xsl:for-each select="point/title">
				<mods:titleInfo>
					<mods:title>
						<xsl:value-of select="."/>
					</mods:title>
				</mods:titleInfo>
			</xsl:for-each>
		</mods:relatedItem>
	</xsl:template>
    
	<xsl:template match="subject" mode="subjects">
		<mods:titleInfo>
			<mods:title>
				<xsl:value-of select="."/>
			</mods:title>
		</mods:titleInfo>
	</xsl:template>
    
	<xsl:template match="*">
		<xsl:apply-templates />
	</xsl:template>
    
	<xsl:template match="text()" />
    
</xsl:stylesheet>