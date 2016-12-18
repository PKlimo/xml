#define _GNU_SOURCE
#include <stdlib.h>  // EXIT_FAILURE, EXIT_SUCCESS
#include <err.h>  // err
#include <string.h>  // strcmp
#include <stdio.h>  // asprintf
#include <sys/stat.h>  // fstat
#include <fcntl.h>  // open
#include <unistd.h>  // close
#include <sys/mman.h>  // mmap, munmap
#include <libxslt/xsltutils.h>
#include <libxslt/transform.h>

// extern int xmlLoadExtDtdDefaultValue;

struct element {
    char *el_beg;  // contains pointer to string "<el>"
    char *el_end;  // contains pointer to string "</el>"
    size_t el_beg_len;  // length of string
    size_t el_end_len;  // length of string
};

int read_xmlfile(char *xslt, char *fname, char *el);
int create_element(char *el, struct element *e);
void free_element(struct element e);


int main(int argc, char *argv[]) {
    if (argc != 4){
        fprintf(stderr, "%s <xslt> <xml> <element>\n", argv[0]);
        return EXIT_FAILURE;
    }

    return read_xmlfile(argv[1], argv[2], argv[3]);
}

int read_xmlfile(char *xsltname, char *fname, char *el) {
    int return_value = EXIT_FAILURE;
    int fd = open (fname, O_RDONLY);
    if (fd == -1) {
        perror ("open");
        goto error;
    }
    struct stat sb;
    if (fstat (fd, &sb) == -1) { // properties of file - we need only size of file, which is in sb.st_size
        perror ("fstat");
        goto cleanup_closefile;
    }
    if (!S_ISREG (sb.st_mode)) {
        fprintf (stderr, "%s is not a file\n", fname);
        goto cleanup_closefile;
    }
    char* chars = mmap (0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);  // content of big xml file
    if (chars == MAP_FAILED) {
        perror ("mmap");
        goto cleanup_closefile;
    }

    struct element e;
    if (create_element(el, &e) == -1) {
        perror("create element");
        goto cleanup_munamp;
    }

    const char *beg_part = NULL;  // pointer to beginning element in big xml (char *chars)
    const char *end_part = NULL;  // pointer to closing element in big xml (char *chars)

	xmlSubstituteEntitiesDefault(1);  // init xslt stuff
	xmlLoadExtDtdDefaultValue = 1;  // init xslt stuff
    xsltStylesheetPtr xslt = NULL;
	xslt = xsltParseStylesheetFile((const xmlChar *)xsltname);
    if (xslt == NULL){
        perror("Can not parse xslt");
        goto cleanup_free_element;
    }
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
    return_value = EXIT_SUCCESS;
// cleanup:
    xsltCleanupGlobals();
    xmlCleanupParser();
	xsltFreeStylesheet(xslt);
cleanup_free_element:
    free_element(e);
cleanup_munamp:
    if (munmap (chars, sb.st_size) == -1)
        perror("munmap");
cleanup_closefile:
    if (close(fd) == -1)
        perror("close");
error:
    return return_value;
}

int create_element(char *el, struct element *e){
    if (asprintf(&e->el_beg, "<%s>", el) != (int)strlen(el)+2) {
        perror("Unable to allocate beginning string element");
        return -1;
    }
    e->el_beg_len = strlen(e->el_beg);

    if (asprintf(&e->el_end, "</%s>", el) != (int)strlen(el)+3){
        perror("Unable to allocate ending string element");
        return -1;
    }
    e->el_end_len = strlen(e->el_end);
    return 0;
}

void free_element(struct element e){
    free(e.el_beg);
    free(e.el_end);
}
