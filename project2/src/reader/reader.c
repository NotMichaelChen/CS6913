#include "reader.h"

#include <stdlib.h>
#include <string.h>

#include "michaellib/string.h"
#include "metareader.h"

struct Reader {
    FILE* fp;
    int status;
};

Reader* reader_new(FILE* fp) {
    Reader* newreader = malloc(sizeof(Reader));
    newreader->fp = fp;
    newreader->status = 0;

    char* line = NULL;
    size_t linelen = 0;
    getline(&line, &linelen, newreader->fp);

    //first line doesn't equal what we expect
    if(strcmp(line, "WARC/1.0\r\n")) {
        newreader->status = 1;
    }
    else {
        //Loop until we find a new document segment
        while(getline(&line, &linelen, newreader->fp) != -1) {
            if(!strcmp(line, "WARC/1.0\r\n"))
                break;
        }
    }

    free(line);
    return newreader;
}

Document reader_getDocument(Reader* reader) {
    if(reader->status != 0)
        return (Document) {NULL, NULL, -1};

    //Read metadata
    ReaderMetadata metadata;
    int err = reader_readMetadata(&metadata, reader->fp);
    if(err) {
        reader->status = 5;
        return (Document) {NULL, NULL, -1};
    }

    //Read document
    int docsize = strtol(metadata.Content_Length, NULL, 10);
    char* doctext = malloc(docsize + 1);
    fread(doctext, 1, docsize, reader->fp);

    //Construct document struct
    Document doc;
    doc.doc = doctext;

    size_t urllen = strlen(metadata.WARC_Target_URI);
    doc.url = malloc(urllen+1);
    strncpy(doc.url, metadata.WARC_Target_URI, urllen + 1);

    doc.docsize = docsize;

    //Read lines until "WARC/1.0\r\n" is found
    char* line = NULL;
    size_t linelen = 0;
    while(getline(&line, &linelen, reader->fp) != -1) {
        if(!strcmp(line, "WARC/1.0\r\n"))
            break;
    }
    free(line);

    reader_freeMetadata(&metadata);

    return doc;
}

int reader_getStatus(Reader* reader) {
    return reader->status;
}

void reader_free(Reader* reader) {
    // We have no responsibilites for the file pointer
    free(reader);
}

void reader_freedoc(Document* doc) {
    free(doc->doc);
    free(doc->url);
}