#include "doctokenizer.h"

#include <string.h>
#include <stdlib.h>

struct DocTokenizer {
    char* tempdoc;
    char* docwalker;
    uint32_t docID;
    int err;
};

DocTokenizer* doctokenizer_new(Document doc, uint32_t docID) {
    DocTokenizer* parser = malloc(sizeof(DocTokenizer));

    parser->docID = 0;

    parser->tempdoc = malloc(doc.docsize + 1);
    strncpy(parser->tempdoc, doc.doc, doc.docsize + 1);

    parser->docwalker = strtok(parser->tempdoc, " \r\n\t");

    parser->err = 0;

    return parser;
}

IntermediatePosting doctokenizer_getPosting(DocTokenizer* parser) {
    IntermediatePosting posting;

    if(parser->docwalker == NULL) {
        parser->err = 1;
        return (IntermediatePosting) {NULL, 0};
    }

    posting.term = string_newstr(parser->docwalker);
    posting.docID = parser->docID;

    parser->docwalker = strtok(parser->tempdoc, " \r\n\t");

    return posting;
}

int doctokenizer_getErr(DocTokenizer* parser) {
    return parser->err;
}

void doctokenizer_free(DocTokenizer* parser) {
    free(parser->tempdoc);
    free(parser);
}