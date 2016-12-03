<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<xsl:output method="text" encoding="utf-8"/>

  <xsl:template match="/">
	  <xsl:apply-templates select="//cd"/>
</xsl:template>

  <xsl:template match="cd">
	  <xsl:value-of select="title"/>
    <xsl:text>
</xsl:text><!--put new line here-->
  </xsl:template>

</xsl:stylesheet>
