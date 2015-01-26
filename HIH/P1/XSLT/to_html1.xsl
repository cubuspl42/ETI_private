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
                <link rel="stylesheet" type="text/css" href="style.css" />
            </head>
            <body>
                <xsl:apply-templates/>
                <footer>
                    XSL, wersja <xsl:value-of select="format-number(system-property('xsl:version'), '0.0#')"/>
                </footer>
            </body>
        </html>
    </xsl:template>
    <xsl:template match="informacje">
        <h1>Informacje o zawodach</h1>
        <table id="informacje">
            <tr><th>Ranga</th>
            <td>
            <xsl:value-of select="$ranga"/>
            <xsl:choose>
                <xsl:when test="$ranga = 'Mistrzostwa Gdańska'"> &#x2605;</xsl:when>
                <xsl:when test="$ranga = 'Mistrzostwa Województwa Pomorskiego'"> &#x2605;&#x2605;</xsl:when>
                <xsl:when test="$ranga = 'Mistrzostwa Polski'"> &#x2605;&#x2605;&#x2605;</xsl:when>
                <xsl:otherwise />
            </xsl:choose>
            </td></tr>
            <tr><th>Data</th><td>
            <xsl:call-template name="data"><xsl:with-param name="datetime" select="./termin" /></xsl:call-template></td></tr>
            <tr><th>Baza</th><td><xsl:value-of select="./baza"/><span class="gps"> (<xsl:value-of select="./baza/@gps"/>)</span></td></tr>
            <tr><th>Organizator</th><td><xsl:apply-templates select="./organizator"/></td></tr>
            <tr><th>Patroni medialni</th><td>
            <xsl:if test="not(./patroni_medialni/*)">(brak patronów medialnych)</xsl:if>
            <xsl:if test="./patroni_medialni/*"><xsl:apply-templates select="./patroni_medialni"/></xsl:if>
            </td></tr>
        </table>
    </xsl:template>
    <xsl:template match="patroni_medialni">
        <ul id="patroni_medialni">
            <xsl:apply-templates mode="custom"/>
        </ul>
    </xsl:template>
    <xsl:template match="patron" mode="custom">
        <li><xsl:apply-templates select="."/></li>
    </xsl:template>
    <xsl:template match="wyniki">
        <h1>Wyniki zawodów</h1>
        <xsl:apply-templates />
    </xsl:template>
    <xsl:template match="kategoria">
        <h2>Kategoria <xsl:value-of select="./@nazwa" /></h2>
        <table class="wyniki">
            <tr><th>Miejsce</th><th>Czas</th><th>Nazwisko</th><th>Imię</th><th>Klub</th><th>Numer PWP</th></tr>
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
        <td><xsl:value-of select="./czas" /></td><td><xsl:value-of select="./nazwisko" /></td><td><xsl:value-of select="./imie" /></td><td><xsl:value-of select="./klub" /></td><td><xsl:value-of select="./nr_pwp" /></td></tr>
    </xsl:template>
    <xsl:template match="mapy">
        <h1>Mapy</h1>
        <ul>
        <xsl:for-each select="./mapa">
            <li><p>Trasa <xsl:number format="A"/></p><xsl:apply-templates select="." mode="custom" /></li>
        </xsl:for-each>
        </ul>
    </xsl:template>
    <xsl:template match="mapy/mapa" mode="custom">
        <a href="{./@lokalizacja}"><img width="200px" alt="{./@trasa}" src="{./@lokalizacja}" /></a>
    </xsl:template>
    <xsl:template match="*">
        UWAGA: Niedopasowany element: <xsl:value-of select="name(current())"/><br/>
    </xsl:template>
    <xsl:template name="data">
        <xsl:param name="datetime" />
        <time datetime="{$datetime}"><xsl:value-of select="$datetime"/></time>
    </xsl:template>
</xsl:stylesheet>