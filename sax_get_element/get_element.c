#include <stdio.h>
#include <string.h>
#include <libxml/SAX.h>

#define UNUSED(x) (void)(x)

int p = 0;
char* el;
unsigned long position = 0;

int read_xmlfile(FILE *f);
xmlSAXHandler make_sax_handler(void);
static void OnStartElementNs(void *ctx,const xmlChar *localname,const xmlChar *prefix,const xmlChar *URI,int nb_namespaces,const xmlChar **namespaces,int nb_attributes,int nb_defaulted,const xmlChar **attributes);
static void OnEndElementNs(void* ctx,const xmlChar* localname,const xmlChar* prefix,const xmlChar* URI);
static void OnCharacters(void* ctx, const xmlChar * ch, int len);

int main(int argc, char *argv[]) {
    if (argc != 3){
       printf("%s <file> <element>\n", argv[0]);
        return -1;
    }
    FILE *f = fopen(argv[1], "r");
    el = argv[2];
    if (!f) {
        puts("file open error.");
        exit(1);
    }

    if(read_xmlfile(f)) {
        puts("xml read error.");
        exit(1);
    }

    fclose(f);
    return 0;
}

int read_xmlfile(FILE *f) {
    char chars[1024];
    int res = fread(chars, 1, 4, f);
    // printf("res:%i\n", res);
    // printf("chars:%s\n", chars);
    if (res <= 0) {
        return 1;
    }

    // printf("data:%s\n", cdcatalog_xml);
    xmlSAXHandler SAXHander = make_sax_handler();

    xmlParserCtxtPtr ctxt = xmlCreatePushParserCtxt(
        &SAXHander, NULL, chars, res, NULL
    );

    while ((res = fread(chars, 1, sizeof(chars), f)) > 0) {
        if(xmlParseChunk(ctxt, chars, res, 0)) {
            xmlParserError(ctxt, "xmlParseChunk");
            return 1;
        }
    }
    xmlParseChunk(ctxt, chars, 0, 1);

    xmlFreeParserCtxt(ctxt);
    xmlCleanupParser();
    return 0;
}

xmlSAXHandler make_sax_handler (){
    xmlSAXHandler SAXHander;

    memset(&SAXHander, 0, sizeof(xmlSAXHandler));

    SAXHander.initialized = XML_SAX2_MAGIC;
    SAXHander.startElementNs = OnStartElementNs;
    SAXHander.endElementNs = OnEndElementNs;
    SAXHander.characters = OnCharacters;

    return SAXHander;
}

static void OnStartElementNs(void *ctx,const xmlChar *localname,const xmlChar *prefix,const xmlChar *URI,int nb_namespaces,const xmlChar **namespaces,int nb_attributes,int nb_defaulted,const xmlChar **attributes) {
    UNUSED(prefix); UNUSED(URI); UNUSED(nb_namespaces); UNUSED(namespaces); UNUSED(nb_attributes); UNUSED(nb_defaulted); UNUSED(attributes); UNUSED(ctx);
    if (strcmp((const char*)localname, el) == 0) {
        // xmlParserCtxt *pok;
        // pok = ctx;
        // printf("[DEBUG] %s %i:%i\n", localname, xmlSAX2GetLineNumber(ctx), xmlSAX2GetColumnNumber(ctx));
        // printf("[DEBUG] %p of %p, diff: %i\n", pok->input->cur, pok->input->base, (pok->input->cur - pok->input->base));
        // printf("[DEBUG] position: %lu\n", position);

        position++;
        p = 1;
    }
}

static void OnEndElementNs(void* ctx,const xmlChar* localname,const xmlChar* prefix,const xmlChar* URI) {
    UNUSED(ctx); UNUSED(prefix); UNUSED(URI);
    if (strcmp((const char *)localname, el) == 0)
        p = 0;
}

static void OnCharacters(void *ctx, const xmlChar *ch, int len) {
    UNUSED(ctx);
    char chars[len + 1];
    strncpy(chars, (const char *)ch, len);
    chars[len] = '\0';
    if (p == 1)
        // printf("%.*s\n", 11, chars+4);  // envelopesequence
        printf("%s\n", chars);
}
