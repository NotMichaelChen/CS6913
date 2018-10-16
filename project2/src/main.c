#include <stdio.h>
#include <string.h>

#include "reader/reader.h"
#include "reader/dirreader.h"
#include "michaellib/string.h"

int main (int argc, char *argv[]) {
    printf("Hello World!\n");

    DirReader* reader = dirreader_new("CC");
    Document doc;

    while(1) {
        doc = dirreader_getDocument(reader);
        if(dirreader_getStatus(reader))
            break;

        printf("%s %i\n", doc.url, doc.docsize);
        reader_freedoc(&doc);
    }

    //IntermediatePostingList list = docparser_getPostings(doc);
    // printf("%i\n", list.len);
    // for(int i = 0; i < list.len; i++) {
    //     printf("%s %i\n", string_getString(list.head[i].term), list.head[i].freq);
    // }

    reader_freedoc(&doc);

    // FILE* fp = fopen("CC-MAIN-20130516092621-00000-ip-10-60-113-184.ec2.internal.warc.wet", "r");
    // if(!fp) {
    //     printf("Error, unable to open file\n");
    //     return 1;
    // }
    // Reader* reader = reader_new(fp);
    // printf("%d\n", reader_getStatus(reader));
    // Document doc = reader_getDocument(reader);
    // //TODO: Figure out how to avoid this when reading new docs?
    // reader_freedoc(&doc);
    // doc = reader_getDocument(reader);
    // printf("%s %i %s\n", doc.url, doc.docsize, doc.doc);
    
    // reader_freedoc(&doc);
    // reader_free(reader);
    // fclose(fp);

    // String* str = string_new();

    // char* next = "def";
    // string_appendString(str, next, strlen(next));
    // printf("%s\n", string_getString(str));

    return 0;
}
