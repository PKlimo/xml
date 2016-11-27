#include <sys/stat.h>  // fstat
#include <fcntl.h>  // open
#include <unistd.h>  // close
#include <sys/mman.h>  // mmap, munmap
#include <string.h>  // strcmp
#include <libxml/SAX.h>

#define UNUSED(x) (void)(x)

int p = 0;
char* el;
unsigned long position = 0;

int read_xmlfile(char* fname);
xmlSAXHandler make_sax_handler(void);
static void OnStartElementNs(void *ctx,const xmlChar *localname,const xmlChar *prefix,const xmlChar *URI,int nb_namespaces,const xmlChar **namespaces,int nb_attributes,int nb_defaulted,const xmlChar **attributes);
static void OnEndElementNs(void* ctx,const xmlChar* localname,const xmlChar* prefix,const xmlChar* URI);

int main(int argc, char *argv[]) {
    if (argc != 3){
       printf("%s <file> <element>\n", argv[0]);
        return -1;
    }
    el = argv[2];

    read_xmlfile(argv[1]);
    return 0;
}

int read_xmlfile(char* fname) {
    int fd = open (fname, O_RDWR);

    struct stat sb;
    fstat (fd, &sb);

    char* chars = mmap (0, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);

    xmlSAXHandler SAXHander = make_sax_handler();
    xmlParserCtxtPtr ctxt = xmlCreatePushParserCtxt(&SAXHander, NULL, chars, sb.st_size, NULL);

    xmlCtxtReadMemory(ctxt, chars, sb.st_size, NULL, NULL, 0);  // call SAX parser

    // free alocated resources
    xmlFreeParserCtxt(ctxt);
    xmlCleanupParser();
    munmap (chars, sb.st_size);
    close(fd);
    return 0;
}

xmlSAXHandler make_sax_handler (){
    xmlSAXHandler SAXHander;

    memset(&SAXHander, 0, sizeof(xmlSAXHandler));

    SAXHander.initialized = XML_SAX2_MAGIC;
    SAXHander.startElementNs = OnStartElementNs;
    SAXHander.endElementNs = OnEndElementNs;

    return SAXHander;
}

static void OnStartElementNs(void *ctx,const xmlChar *localname,const xmlChar *prefix,const xmlChar *URI,int nb_namespaces,const xmlChar **namespaces,int nb_attributes,int nb_defaulted,const xmlChar **attributes) {
    UNUSED(prefix); UNUSED(URI); UNUSED(nb_namespaces); UNUSED(namespaces); UNUSED(nb_attributes); UNUSED(nb_defaulted); UNUSED(attributes); UNUSED(ctx);
    if (strcmp((const char*)localname, el) == 0) {
        xmlParserCtxt *pok;
        pok = ctx;
        printf("[DEBUG] %s %i:%i\n", localname, xmlSAX2GetLineNumber(ctx), xmlSAX2GetColumnNumber(ctx));
        printf("[DEBUG] %p of %p, diff: %i\n", pok->input->cur, pok->input->base, (pok->input->cur - pok->input->base));
        printf("[DEBUG] position: %lu\n", position);

        position++;
        p = 1;
    }
}

static void OnEndElementNs(void* ctx,const xmlChar* localname,const xmlChar* prefix,const xmlChar* URI) {
    UNUSED(ctx); UNUSED(prefix); UNUSED(URI);
    if (strcmp((const char *)localname, el) == 0)
        p = 0;
}
