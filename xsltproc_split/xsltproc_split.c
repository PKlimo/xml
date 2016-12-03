#include <sys/stat.h>  // fstat
#include <fcntl.h>  // open
#include <unistd.h>  // close
#include <sys/mman.h>  // mmap, munmap
#include <string.h>  // strcmp
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

extern int xmlLoadExtDtdDefaultValue;

struct element {
    char *el_beg;  // contains pointer to string "<el>"
    char *el_end;  // contains pointer to string "</el>"
    size_t el_beg_len;  // length of string
    size_t el_end_len;  // length of string
};

int read_xmlfile(char *xslt, char *fname, char *el);
void create_element(char *el, struct element *e);
void free_element(struct element e);


int main(int argc, char *argv[]) {
    if (argc != 4){
       printf("%s <xslt> <xml> <element>\n", argv[0]);
        return -1;
    }

    read_xmlfile(argv[1], argv[2], argv[3]);
    return 0;
}

int read_xmlfile(char *xsltname, char *fname, char *el) {
    int fd = open (fname, O_RDWR);
    struct stat sb;
    fstat (fd, &sb);  // properties of file - we need only size of file, which is in sb.st_size
    char* chars = mmap (0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);  // content of big xml file

    struct element e;
    create_element(el, &e);

    const char *beg_part = NULL;  // pointer to beginning element in big xml (char *chars)
    const char *end_part = NULL;  // pointer to closing element in big xml (char *chars)

	xmlSubstituteEntitiesDefault(1);  // init xslt stuff
	xmlLoadExtDtdDefaultValue = 1;  // init xslt stuff
    xsltStylesheetPtr xslt = NULL;
	xslt = xsltParseStylesheetFile((const xmlChar *)xsltname);
    xmlDocPtr xml_in, xml_out;

    for (off_t i=0; i<sb.st_size;i++){
        if (beg_part == NULL) {  // we are searching for beginning element
            if (strncmp(e.el_beg, chars+i, e.el_beg_len) == 0){
                beg_part = chars + i;
                //printf("%.10s\n", beg_part);
            }
        } else {  // we have pointer that points to beginning of section, thus we are searchingg for end element
            if (strncmp(e.el_end, chars+i, e.el_end_len) == 0) {
                end_part = chars + i;
                // printf("%.*s\n\n", end_part-beg_part+el_end_len, beg_part);  // DEBUG
                // do xslt transformation on part from beginning element to end element
                xml_in = xmlReadMemory(beg_part, end_part-beg_part+e.el_end_len, NULL, "utf-8", XML_PARSE_COMPACT|XML_PARSE_HUGE);
                xml_out = xsltApplyStylesheet(xslt, xml_in, NULL);
                xsltSaveResultToFile(stdout, xml_out, xslt);
                beg_part = NULL;
                xmlFreeDoc(xml_out);
                xmlFreeDoc(xml_in);
            }
        }
    }
    // free alocated resources
    free_element(e);
	xsltFreeStylesheet(xslt);
    xsltCleanupGlobals();
    xmlCleanupParser();
    munmap (chars, sb.st_size);
    close(fd);
    return 0;
}

void create_element(char *el, struct element *e){
    e->el_beg = (char *)malloc(strlen(el)+3);
    strcpy(e->el_beg,"<");
    strcat(strcat(e->el_beg, el), ">");

    e->el_end = (char *)malloc(strlen(el)+4);
    strcpy(e->el_end,"</");
    strcat(strcat(e->el_end, el), ">");

    e->el_beg_len = strlen(e->el_beg);
    e->el_end_len = strlen(e->el_end);
}

void free_element(struct element e){
    free(e.el_beg);
    free(e.el_end);
}
