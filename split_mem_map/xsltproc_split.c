#include <sys/stat.h>  // fstat
#include <fcntl.h>  // open
#include <unistd.h>  // close
#include <sys/mman.h>  // mmap, munmap
#include <string.h>  // strcmp
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

extern int xmlLoadExtDtdDefaultValue;

int read_xmlfile(char *xslt, char *fname, char *el);

int main(int argc, char *argv[]) {
    if (argc != 4){
       printf("%s <xslt> <xml> <element>\n", argv[0]);
        return -1;
    }

    read_xmlfile(argv[1], argv[2], argv[3]);
    return 0;
}

int read_xmlfile(char *xsltname, char *fname, char *el) {
    xmlDocPtr xml_in, xml_out;
    const char *beg_part=NULL;
    const char *end_part=NULL;

    char *el_beg = (char *)malloc(strlen(el)+3);
    strcpy(el_beg,"<");
    strcat(strcat(el_beg, el), ">");

    char *el_end = (char *)malloc(strlen(el)+4);
    strcpy(el_end,"</");
    strcat(strcat(el_end, el), ">");

    size_t el_beg_len = strlen(el_beg);
    size_t el_end_len = strlen(el_end);

    int fd = open (fname, O_RDWR);

    struct stat sb;
    fstat (fd, &sb);

    char* chars = mmap (0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);

	xmlSubstituteEntitiesDefault(1);
	xmlLoadExtDtdDefaultValue = 1;
    xsltStylesheetPtr xslt = NULL;
	xslt = xsltParseStylesheetFile((const xmlChar *)xsltname);

    for (off_t i=0; i<sb.st_size;i++){
        if (beg_part == NULL) {
            if (strncmp(el_beg, chars+i, el_beg_len) == 0){
                beg_part = chars + i;
                //printf("%.10s\n", beg_part);
            }
        } else {
            if (strncmp(el_end, chars+i, el_end_len) == 0) {
                end_part = chars + i;
                // printf("%.*s\n\n", end_part-beg_part+el_end_len, beg_part);
                xml_in = xmlReadMemory(beg_part, end_part-beg_part+el_end_len, NULL, "utf-8", XML_PARSE_COMPACT|XML_PARSE_HUGE);
                xml_out = xsltApplyStylesheet(xslt, xml_in, NULL);
                xsltSaveResultToFile(stdout, xml_out, xslt);
                beg_part = NULL;
                xmlFreeDoc(xml_out);
                xmlFreeDoc(xml_in);
            }
        }
    }

    // free alocated resources
	xsltFreeStylesheet(xslt);
    xsltCleanupGlobals();
    xmlCleanupParser();
    munmap (chars, sb.st_size);
    close(fd);
    return 0;
}
