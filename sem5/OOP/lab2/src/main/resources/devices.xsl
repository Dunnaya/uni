<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:output method="html" indent="yes" encoding="UTF-8"/>
    
    <xsl:template match="/">
        <html>
            <head>
                <title>Computer Components Report</title>
                <style>
                    body { font-family: Arial, sans-serif; margin: 20px; }
                    table { border-collapse: collapse; width: 100%; }
                    th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
                    th { background-color: #f2f2f2; }
                    .critical { background-color: #ffebee; }
                    .non-critical { background-color: #e8f5e8; }
                </style>
            </head>
            <body>
                <h1>Computer Components Inventory</h1>
                <table>
                    <thead>
                        <tr>
                            <th>ID</th>
                            <th>Name</th>
                            <th>Origin</th>
                            <th>Price</th>
                            <th>Power Consumption</th>
                            <th>Component Group</th>
                            <th>Ports</th>
                            <th>Critical</th>
                        </tr>
                    </thead>
                    <tbody>
                        <xsl:for-each select="devices/device">
                            <xsl:sort select="price" data-type="number" order="descending"/>
                            <tr>
                                <xsl:attribute name="class">
                                    <xsl:choose>
                                        <xsl:when test="critical = 'true'">critical</xsl:when>
                                        <xsl:otherwise>non-critical</xsl:otherwise>
                                    </xsl:choose>
                                </xsl:attribute>
                                <td><xsl:value-of select="@id"/></td>
                                <td><xsl:value-of select="name"/></td>
                                <td><xsl:value-of select="origin"/></td>
                                <td>$<xsl:value-of select="format-number(price, '#,##0.00')"/></td>
                                <td><xsl:value-of select="type/powerConsumption"/>W</td>
                                <td><xsl:value-of select="type/componentGroup"/></td>
                                <td>
                                    <xsl:for-each select="type/ports/port">
                                        <xsl:value-of select="."/>
                                        <xsl:if test="position() != last()">, </xsl:if>
                                    </xsl:for-each>
                                </td>
                                <td>
                                    <xsl:choose>
                                        <xsl:when test="critical = 'true'">Yes</xsl:when>
                                        <xsl:otherwise>No</xsl:otherwise>
                                    </xsl:choose>
                                </td>
                            </tr>
                        </xsl:for-each>
                    </tbody>
                </table>
                
                <h2>Summary</h2>
                <p>Total devices: <xsl:value-of select="count(devices/device)"/></p>
                <p>Critical devices: <xsl:value-of select="count(devices/device[critical='true'])"/></p>
                <p>Total value: $<xsl:value-of select="format-number(sum(devices/device/price), '#,##0.00')"/></p>
                <p>Average power consumption: <xsl:value-of select="format-number(sum(devices/device/type/powerConsumption) div count(devices/device), '#0.0')"/>W</p>
            </body>
        </html>
    </xsl:template>
</xsl:stylesheet>