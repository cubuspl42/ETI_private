<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"> 
    <xsl:variable name="ranga" select="/zawody/informacje/ranga"/>
    <xsl:template match="/zawody">
        <wyniki>
            <xsl:apply-templates/>
        </wyniki>
    </xsl:template>
    <xsl:template match="wyniki">
        <xsl:apply-templates />
    </xsl:template>
    <xsl:template match="kategoria">
        <xsl:element name="{./@nazwa}">
            <xsl:apply-templates />
        </xsl:element>
    </xsl:template>
    <xsl:template match="kategoria/zawodnik">
        <xsl:element name="{./imie}">
            <xsl:attribute name="{./nazwisko}" />
            <xsl:apply-templates />
            <xsl:copy-of select="./nr_pwp" />
        </xsl:element>
    </xsl:template>
    <xsl:template match="czas|start|meta">
        <xsl:copy><xsl:apply-templates/></xsl:copy>
    </xsl:template>
    <xsl:template match="*" />
</xsl:stylesheet>