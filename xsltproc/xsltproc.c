#include <string.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

extern int xmlLoadExtDtdDefaultValue;

int main(int argc, char **argv) {
    if (argc != 3){
        return(-1);
    }

	xmlSubstituteEntitiesDefault(1);
	xmlLoadExtDtdDefaultValue = 1;

	xsltStylesheetPtr xslt = NULL;
	xslt = xsltParseStylesheetFile((const xmlChar *)argv[1]);

	xmlDocPtr xml_in, xml_out;
	xml_in = xmlParseFile(argv[2]);
	xml_out = xsltApplyStylesheet(xslt, xml_in, NULL);

	xsltSaveResultToFile(stdout, xml_out, xslt);

	xsltFreeStylesheet(xslt);
	xmlFreeDoc(xml_out);
	xmlFreeDoc(xml_in);
    xsltCleanupGlobals();
    xmlCleanupParser();

	return(0);
}
