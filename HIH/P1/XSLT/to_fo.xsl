<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="1.0"
    xmlns="http://www.w3.org/1999/xhtml"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:fo="http://www.w3.org/1999/XSL/Format">
    <xsl:template match="/zawody">
        <fo:root xmlns:fo="http://www.w3.org/1999/XSL/Format">
            <fo:layout-master-set>
                <fo:simple-page-master master-name="informacje">
                    <fo:region-body margin="0.8in" />
                    <fo:region-before />
                    <fo:region-after extent="1.5cm" />
                </fo:simple-page-master>
                <fo:simple-page-master master-name="wyniki">
                    <fo:region-body margin="0.8in" />
                    <fo:region-before />
                    <fo:region-after extent="1.5cm" />
                </fo:simple-page-master>
                <fo:simple-page-master master-name="mapy">
                    <fo:region-body margin-top="0.6in" margin-left="0.7in" margin-right="0.7in" />
                    <fo:region-before />
                    <fo:region-after extent="1.5cm" />
                </fo:simple-page-master>
            </fo:layout-master-set>
            <xsl:apply-templates />
        </fo:root>
    </xsl:template>
    <xsl:template match="informacje">
        <fo:page-sequence master-reference="informacje">
            <fo:static-content flow-name="xsl-region-before">
                <fo:block margin-top="0.3in" font-weight="bold" text-align="center">Informacje o zawodach</fo:block>
            </fo:static-content>
            <fo:static-content flow-name="xsl-region-after">
                <fo:block text-align="center"><fo:page-number /></fo:block>
            </fo:static-content>
            <fo:flow flow-name="xsl-region-body">
                <fo:table>
                    <fo:table-column column-width="50mm"/>
                    <fo:table-column column-width="100mm"/>
                    <fo:table-body>
                        <fo:table-row>
                            <fo:table-cell>
                                <fo:block font-weight="bold">Ranga</fo:block>
                            </fo:table-cell>
                            <fo:table-cell>
                                <fo:block><xsl:value-of select="./ranga"/></fo:block>
                            </fo:table-cell>
                        </fo:table-row>
                        <fo:table-row>
                            <fo:table-cell>
                                <fo:block font-weight="bold">Data</fo:block>
                            </fo:table-cell>
                            <fo:table-cell>
                                <fo:block><xsl:value-of select="./termin"/></fo:block>
                            </fo:table-cell>
                        </fo:table-row>
                        <fo:table-row>
                            <fo:table-cell>
                                <fo:block font-weight="bold">Baza</fo:block>
                            </fo:table-cell>
                            <fo:table-cell>
                                <fo:block><xsl:value-of select="./baza"/></fo:block>
                            </fo:table-cell>
                        </fo:table-row>
                        <fo:table-row>
                            <fo:table-cell>
                                <fo:block font-weight="bold">Organizator</fo:block>
                            </fo:table-cell>
                            <fo:table-cell>
                                <fo:block><xsl:value-of select="./organizator"/></fo:block>
                            </fo:table-cell>
                        </fo:table-row>
                        <fo:table-row>
                            <fo:table-cell>
                                <fo:block font-weight="bold">Patroni medialni</fo:block>
                            </fo:table-cell>
                            <fo:table-cell>
                                <xsl:apply-templates select="./patroni_medialni" />
                            </fo:table-cell>
                        </fo:table-row>
                    </fo:table-body>
                </fo:table>
            </fo:flow>
        </fo:page-sequence>
    </xsl:template>
    <xsl:template match="patroni_medialni">
        <fo:list-block>
            <xsl:apply-templates />
        </fo:list-block>
    </xsl:template>
    <xsl:template match="patron">
        <fo:list-item>
            <fo:list-item-label>
                <fo:block></fo:block>
            </fo:list-item-label>
            <fo:list-item-body>
                <fo:block><xsl:value-of select="." /></fo:block>
            </fo:list-item-body>
        </fo:list-item>
    </xsl:template>
    <xsl:template match="wyniki">
        <fo:page-sequence master-reference="wyniki">
            <fo:static-content flow-name="xsl-region-before">
                <fo:block margin-top="0.3in" font-weight="bold" text-align="center">Wyniki zawodów</fo:block>
            </fo:static-content>
            <fo:static-content flow-name="xsl-region-after">
                <fo:block text-align="center"><fo:page-number /></fo:block>
            </fo:static-content>
            <fo:flow flow-name="xsl-region-body">
                <xsl:apply-templates />
            </fo:flow>
        </fo:page-sequence>
    </xsl:template>
    <xsl:attribute-set name="black_border">
        <xsl:attribute name="border">solid 0.2mm black</xsl:attribute>
    </xsl:attribute-set>
    <xsl:template match="kategoria">
        <fo:block margin="0.2in" text-decoration="underline">Kategoria <xsl:value-of select="./@nazwa" /></fo:block>
        <fo:table>
            <fo:table-column column-width="20mm"/>
            <fo:table-column column-width="15mm"/>
            <fo:table-column column-width="35mm"/>
            <fo:table-column column-width="25mm"/>
            <fo:table-column column-width="50mm"/>
            <fo:table-column column-width="20mm"/>
            <fo:table-header>
                <fo:table-cell xsl:use-attribute-sets="black_border">
                    <fo:block font-weight="bold">Miejsce</fo:block>
                </fo:table-cell>
                <fo:table-cell xsl:use-attribute-sets="black_border">
                    <fo:block font-weight="bold">Czas</fo:block>
                </fo:table-cell>
                <fo:table-cell xsl:use-attribute-sets="black_border">
                    <fo:block font-weight="bold">Nazwisko</fo:block>
                </fo:table-cell>
                <fo:table-cell xsl:use-attribute-sets="black_border">
                    <fo:block font-weight="bold">Imię</fo:block>
                </fo:table-cell>
                <fo:table-cell xsl:use-attribute-sets="black_border">
                    <fo:block font-weight="bold">Klub</fo:block>
                </fo:table-cell>
                <fo:table-cell xsl:use-attribute-sets="black_border">
                    <fo:block font-weight="bold">nr PWP</fo:block>
                </fo:table-cell>
            </fo:table-header>
            <fo:table-body>
                <xsl:apply-templates />
            </fo:table-body>
        </fo:table>
    </xsl:template>
    <xsl:template match="kategoria/zawodnik">
        <fo:table-row>
            <fo:table-cell xsl:use-attribute-sets="black_border">
                <fo:block>
                    <xsl:element name="fo:inline">
                        <xsl:if test="./@miejsce = 'NKL'">
                            <xsl:attribute name="background-color">red</xsl:attribute>
                        </xsl:if>
                        <xsl:value-of select="./@miejsce" />
                    </xsl:element>
                </fo:block>
            </fo:table-cell>
            <fo:table-cell xsl:use-attribute-sets="black_border">
                <fo:block><xsl:value-of select="./czas" /></fo:block>
            </fo:table-cell>
            <fo:table-cell xsl:use-attribute-sets="black_border">
                <fo:block><xsl:value-of select="./nazwisko" /></fo:block>
            </fo:table-cell>
            <fo:table-cell xsl:use-attribute-sets="black_border">
                <fo:block><xsl:value-of select="./imie" /></fo:block>
            </fo:table-cell>
            <fo:table-cell xsl:use-attribute-sets="black_border">
                <fo:block><xsl:value-of select="./klub" /></fo:block>
            </fo:table-cell>
            <fo:table-cell xsl:use-attribute-sets="black_border">
                <fo:block><xsl:value-of select="./nr_pwp" /></fo:block>
            </fo:table-cell>
        </fo:table-row>
    </xsl:template>
    <xsl:template match="mapy">
        <fo:page-sequence master-reference="mapy">
            <fo:static-content flow-name="xsl-region-before">
                <fo:block margin-top="0.3in" font-weight="bold" text-align="center">Mapy</fo:block>
            </fo:static-content>
            <fo:static-content flow-name="xsl-region-after">
                <fo:block text-align="center"><fo:page-number /></fo:block>
            </fo:static-content>
            <fo:flow flow-name="xsl-region-body">
                <xsl:apply-templates />
            </fo:flow>
        </fo:page-sequence>
    </xsl:template>
    <xsl:template match="mapy/mapa">
        <fo:block>
            <fo:external-graphic content-width="18cm" src='url("{./@lokalizacja}")' />
        </fo:block>
    </xsl:template>
</xsl:stylesheet>
