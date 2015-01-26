<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fn="http://www.w3.org/2005/xpath-functions"> 
    <xsl:template match="*[@url]">
        <a href="{@url}"><xsl:value-of select="." /></a>
    </xsl:template>
    <xsl:variable name="ranga" select="/zawody/informacje/ranga"/>
    <xsl:template match="/zawody">
        <html>
            <head>
                <title><xsl:value-of select="$ranga"/> (<xsl:value-of select="./informacje/termin"/>)</title>
                <link rel="stylesheet" type="text/css" href="style2.css" />
            </head>
            <body>
                <xsl:apply-templates/>
                <footer>
                    XSL, wersja <xsl:value-of select="format-number(system-property('xsl:version'), '0.0#')"/>
                </footer>
            </body>
        </html>
    </xsl:template>
    <xsl:template match="wyniki">
        <h1>Skrócone wyniki zawodów</h1>
        <xsl:apply-templates />
    </xsl:template>
    <xsl:template match="kategoria">
        <h2>Kategoria <xsl:value-of select="./@nazwa" /></h2>
        <table class="wyniki">
            <tr><th>Miejsce</th><th>Nazwisko</th><th>Imię</th></tr>
            <xsl:for-each select="./zawodnik">
                <xsl:sort select="./@miejsce" />
                <xsl:comment> numer zawodnika w pliku XML: <xsl:number format="[1] "/></xsl:comment>
                <xsl:apply-templates select="." mode="custom" />
            </xsl:for-each>
        </table>
    </xsl:template>
    <xsl:template match="kategoria/zawodnik" mode="custom">
        <tr>
        <xsl:element name="td">
            <xsl:if test="./@miejsce = 'NKL'">
                <xsl:attribute name="class">nkl</xsl:attribute>
            </xsl:if>
            <xsl:value-of select="./@miejsce" />
        </xsl:element>
        <td><xsl:value-of select="./nazwisko" /></td><td><xsl:value-of select="./imie" /></td>
    </tr>
    </xsl:template>
    <xsl:template match="*">
        <apply-templates />
    </xsl:template>
</xsl:stylesheet>