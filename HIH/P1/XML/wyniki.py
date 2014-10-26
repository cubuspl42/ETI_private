import csv
import xml.etree.ElementTree as et
from xml.dom import minidom

reader = csv.reader(open('wyniki.csv'))
header = None
wyniki = et.Element('wyniki')
kategoria = None
kat_code = ""
k = 0
for i, row in enumerate(reader):
    # Save header row.
    # row = [row[0]]+row[4:]+row[1:4]
    if i == 0:
        header = row
        k = header.index('kategoria')
    else:
        if kat_code != row[k]:
            kat_code = row[k]
            kategoria = et.SubElement(wyniki, 'kategoria')
            kategoria.attrib['nazwa'] = kat_code
        z = et.SubElement(kategoria, 'zawodnik')
        for j, col in enumerate(row):
            if col in ('x', '?', 'niezrzeszony') or j == k:
                continue
            if col == 'HARPAGAN':
                col = 'PKO Harpagan Gdańsk'
            if header[j] == 'miejsce':
                z.attrib['miejsce'] = col
            else:
                e = et.SubElement(z, header[j])
                e.text = col
s = et.tostring(wyniki, 'utf-8')
document = minidom.parseString(s).toprettyxml(indent=' '*4)
print(document)