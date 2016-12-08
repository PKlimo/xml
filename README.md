# xml

this repo contains utilities based on libxslt and libxml writen in C and designed to work with huge xml files

## `repair_xml`

* *NAME* `repair_xml` - repair incomplete xml file
* *SYNOPSIS* `repair_xml file.xml "</section>" "</file>"`
    * `file.xml` xml file, that is modified
    * `</section>` closing section element - find last occurence of this element
    * `</file>` closing file element - add this tag after last `</section>` element
* *USAGE*
    * huge xml file is truncated via dd `dd if=huge.xml bs=1 count=2M of=preview.xml`
    * truncated xml is repaired by utility e.g.
        * `repair_xml </cd> </catalog>`
        * `repair_xml </letter> </response>`
    * file is inplace repaired (via memory map) to valid xml

## `sax_get_element`

* *NAME* `sax_get_element` - by using SAX parser, print value / content of given element
* *SYNOPSIS* `sax_get_element file.xml element`
    * `file.xml` input xml file
    * `element` element whose content will be printed
* *USAGE* `sax_get_element file.xml title`
    * print on standard output content inside tag <title></title> - position independent i.e. //title
    * `file.xml` xml file, that is modified

## `xsltproc_split`

* *NAME* `xsltproc_split` - split huge xml file according given element and performs xslt transformation on each part
* *SYNOPSIS* `xsltproc_split transformation.xslt file.xml element`
    * `transformation.xslt` - transformation - must not contain position (or outside section content dependancies)
    * `file.xml` input xml file
    * `element` split xml into parts according this element
* *USAGE* `xsltproc cdcatalog.xslt cdcatalog.xml cd`
    * perfom xslt transformation on each subtree cd and print output to stdout
