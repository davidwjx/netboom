<?xml version="1.0" encoding="iso-8859-1"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:ts="http://check.sourceforge.net/ns">

<!--
 Transformation of testrunner,pl results for viewing with a web browser.

 File: check-to-xhtml.xsl

 Example file that transforms test results from testrunner.pl into
 XHTML for displaying with modern web browser.

 As input takes file of format:
    <testsuites-ref>
        <href>suite1.xml</href>
        <href>suite2.xml</href>
    </testsuites-ref>

 referencing all the XML test suite results to view on this page.

 Copyright (c) 2008-2018 INSIDE Secure B.V. All Rights Reserved.

 This confidential and proprietary software may be used only as authorized
 by a licensing agreement from INSIDE Secure.

 The entire notice above must be reproduced on all authorized copies that
 may only be made to the extent permitted by a licensing agreement from
 INSIDE Secure.

 For more information or support, please go to our online support system at
 https://customersupport.insidesecure.com.
 In case you do not have an account for this system, please send an e-mail
 ESSEmbeddedHW-Support@insidesecure.com.
-->

<xsl:template match="ts:suite">
<xsl:variable name="id" select="generate-id(.)"/>
<xsl:variable name="success" select="count(ts:test[@result='success'])"/>
<xsl:variable name="unsupported" select="count(ts:test[@result='unsupported'])"/>
<xsl:variable name="error" select="count(ts:test[@result='error']) + count(ts:test[@result='timeout'])"/>
<xsl:variable name="all" select="count(ts:test) - count(ts:test[@result='unsupported'])"/>
<xsl:variable name="display"><xsl:if test="$success = $all">display: none;</xsl:if></xsl:variable>
<xsl:variable name="not-display"><xsl:if test="$success != $all">display: none;</xsl:if></xsl:variable>
<xsl:variable name="summary">
  <xsl:choose>
    <xsl:when test="$error &gt; 0">error</xsl:when>
    <xsl:when test="$success = $all and $unsupported &gt; 0">unsupported</xsl:when>
    <xsl:when test="$success = $all">success</xsl:when>
    <xsl:otherwise>failure</xsl:otherwise>
  </xsl:choose>
</xsl:variable>
<br/><xsl:element name="input">
<xsl:attribute name="type">checkbox</xsl:attribute>
<xsl:attribute name="class">fold</xsl:attribute>
  <xsl:choose>
    <xsl:when test="$error &gt; 0">
      <xsl:attribute name="checked">checked</xsl:attribute>
    </xsl:when>
    <xsl:when test="$success = $all and $unsupported &gt; 0">
    </xsl:when>
    <xsl:when test="$success = $all">
    </xsl:when>
    <xsl:otherwise>
      <xsl:attribute name="checked">checked</xsl:attribute>
    </xsl:otherwise>
  </xsl:choose>
</xsl:element><table class="java">
<thead>
<xsl:element name="tr">
 <xsl:attribute name="class">
 <xsl:choose>
    <xsl:when test="$error &gt; 0">error</xsl:when>
    <xsl:when test="$success = $all and $unsupported &gt; 0">unsupported</xsl:when>
    <xsl:when test="$success = $all">success</xsl:when>
    <xsl:otherwise>failure</xsl:otherwise>
  </xsl:choose>
 </xsl:attribute>
<th colspan="4"><xsl:value-of select="ts:title"/></th>
<th align="right"><xsl:value-of select="$success"/>/<xsl:value-of select="$all"/><xsl:if test="$unsupported &gt; 0">&#32;(unsupported:&#32;<xsl:value-of select="$unsupported"/>)</xsl:if></th>
</xsl:element>
</thead>
<xsl:for-each select="ts:test">
  <xsl:element name="tr">
    <xsl:if test="@result = 'error'"><xsl:attribute name="class">error</xsl:attribute><td>ERR</td></xsl:if>
    <xsl:if test="@result = 'timeout'"><xsl:attribute name="class">error</xsl:attribute><td>TIME</td></xsl:if>
    <xsl:if test="@result = 'failure'"><xsl:attribute name="class">failure</xsl:attribute><td>FAIL</td></xsl:if>
    <xsl:if test="@result = 'success'"><xsl:attribute name="class">success</xsl:attribute><td>OK</td></xsl:if>
    <xsl:if test="@result = 'unsupported'"><xsl:attribute name="class">unsupported</xsl:attribute><td>-</td></xsl:if>
    <td colspan="2"><xsl:value-of select="ts:path"/>:&#8203;<xsl:value-of select="ts:fn"/>:&#8203;<xsl:value-of select="ts:id"/>:<xsl:value-of select="ts:iteration"/></td>
    <td colspan="2"><xsl:value-of select="ts:description"/><xsl:text>&#32;</xsl:text><xsl:value-of select="ts:message"/></td>
  </xsl:element>
  </xsl:for-each>
</table>
</xsl:template>
<xsl:template match="testsuites">
  <xsl:for-each select="//suite">
    <xsl:apply-templates select="."/>
  </xsl:for-each>
</xsl:template>

<xsl:template match="testsuites-ref">
<html>
<head>
<title>Unit test results</title>
<style type="text/css">
/* Table style, looks bit like java, has fixed layout. */
table.java {
 width: 90%;
 border-width: 1px 0px 0px 1px;
 border-spacing: 0px;
 border-style: solid;
 border-color: gray;
 border-collapse: separate;
 background-color: white;
}
/* Use sans-serif font. */
table.java th,td { font-family: Arial, Helvetica, sans-serif;
                   font-size: 10pt; font-weight: 500; }
/* Style for table heading */
table.java thead tr th {
 border: 1px;
 padding: 1px 2px;
 border-style: outset outset outset outset;
 border-color: rgb(250, 250, 250) rgb(250, 250, 250) rgb(250, 250, 250) rgb(250, 250, 250);
}
/* Style for non-heading cells */
table.java tr td {
 border-width: 0px 1px 1px 0px;
 padding: 1px 2px;
 border-style: solid;
 border-color: gray;
}
/* Body color that goes with table colors. */
body { background-color: rgb(224, 224, 224); }
/* Style table as inline to allow folding box on same line. */
table.java { display: inline-table; }
/* Table heading */
table.java th { text-align: left }
/* Styling: Remove overflow instead of long line. */
table.java tr.nowrap th { white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }

/* Color schemes for table internals. */
table.java tr.unsupported { background-color: rgb(255, 255, 255); }
table.java tr.error { background-color: rgb(256, 210, 210); }
table.java tr.success { background-color: rgb(210, 256, 210); }
table.java tr.failure { background-color: rgb(256, 256, 210); }

table.java tr.unsupported:hover { background-color: rgb(224, 224, 224); }
table.java tr.error:hover { background-color: rgb(2224, 190, 190); }
table.java tr.success:hover { background-color: rgb(190, 224, 190); }
table.java tr.failure:hover { background-color: rgb(224, 224, 190); }

/* Color schemes for table headings. */
table.java thead tr.unsupported { background-color: rgb(192, 192, 192); }
table.java thead tr.error { background-color: rgb(256, 192, 192); }
table.java thead tr.success { background-color: rgb(192, 256, 192); }
table.java thead tr.failure { background-color: rgb(240, 240, 192); }

input[type="checkbox"].fold                                { cursor: S-resize; clear: left; float: left; }
/* Cursor: Use Mozilla extensions if available. */
input[type="checkbox"].fold:not(:checked)                  { cursor: -moz-zoom-in }
input[type="checkbox"].fold:checked                        { cursor: -moz-zoom-out }
/* Hide all except header for folded. */
input[type="checkbox"].fold:not(:checked) + table tr       { display: none }
input[type="checkbox"].fold:checked + table tr             { display: table-row }
input[type="checkbox"].fold:not(:checked) + table thead tr { display: table-row }
/* Fold element: hide when CSS2.1 pseudo-class fold is not supported. */
input[type="checkbox"].fold { display: none }
input[type="checkbox"].fold:not(:checked) { display: inline; }
input[type="checkbox"].fold:checked { display: inline; }
/* Style folding checkbox (Mozilla specific extension.) */
input[type="checkbox"].fold:checked { -moz-appearance: scrollbarbutton-down !important; }
input[type="checkbox"].fold { -moz-appearance: scrollbarbutton-right !important; }
</style>
</head>
<body>
<form>
  <xsl:if test="@href">
    <xsl:variable name="filename" select="@href"/>
    <xsl:for-each select="document($filename)">
      <xsl:for-each select="ts:testsuites">
        <xsl:apply-templates select="ts:suite"/>
      </xsl:for-each>
    </xsl:for-each>
  </xsl:if>
  <xsl:for-each select="href">
    <xsl:variable name="filename" select="text()"/>
    <xsl:for-each select="document($filename)">
      <xsl:for-each select="ts:testsuites">
        <xsl:apply-templates select="ts:suite"/>
      </xsl:for-each>
    </xsl:for-each>
  </xsl:for-each>
</form>
</body>
</html>
</xsl:template>


</xsl:stylesheet>
