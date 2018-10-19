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

    // Get the first line of the document
    char* line = NULL;
    size_t linelen = 0;
    getline(&line, &linelen, newreader->fp);

    // Set error if first line doesn't equal what we expect
    if(strcmp(line, "WARC/1.0\r\n")) {
        newreader->status = 1;
    }
    // Otherwise, skip the file metadata
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
    // If the reader is invalid, don't attempt to read anything
    if(reader->status != 0)
        return (Document) {NULL, NULL, -1};

    // If metadata read failed, set error
    ReaderMetadata metadata;
    int err = reader_readMetadata(&metadata, reader->fp);
    if(err) {
        reader->status = 5;
        return (Document) {NULL, NULL, -1};
    }

    // Read document size from the metadata Content_Length field
    int docsize = strtol(metadata.Content_Length, NULL, 10);
    // Read the rest of the document from the WET file using the docsize var
    char* doctext = malloc(docsize + 1);
    fread(doctext, 1, docsize, reader->fp);
    doctext[docsize] = '\0';

    //Construct document struct
    Document doc;
    doc.doc = doctext;

    // Set the url by copying it from the metadata struct
    size_t urllen = strlen(metadata.WARC_Target_URI);
    doc.url = malloc(urllen+1);
    strcpy(doc.url, metadata.WARC_Target_URI);

    // Set docsize
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