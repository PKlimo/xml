#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import xml.sax

class MyHandler(xml.sax.ContentHandler):
    def __init__(self):
        xml.sax.ContentHandler.__init__(self)
        self.p = False

    def startElement(self, name, attrs):
        if name == "title":
            self.p = True

    def endElement(self, name):
        if name == "title":
            self.p = False

    def characters(self, content):
        if self.p:
            print(content)

def main():
    xml.sax.parse(open('cdcatalog.xml'), MyHandler())

if __name__ == "__main__":
    main()
