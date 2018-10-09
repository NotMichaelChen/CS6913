#include <stdio.h>
#include <string.h>

#include "reader/reader.h"
#include "michaellib/string.h"

int main (int argc, char *argv[]) {
    printf("Hello World!\n");

    FILE* fp = fopen("CC-MAIN-20180317035630-20180317055630-00000.warc.wet", "r");
    Reader* reader = reader_new(fp);
    printf("%d\n", reader_getStatus(reader));
    Document doc = reader_getDocument(reader);
    //TODO: Figure out how to avoid this when reading new docs?
    reader_freedoc(&doc);
    doc = reader_getDocument(reader);
    printf("%s %s\n", doc.url, doc.doc);
    
    reader_freedoc(&doc);
    reader_free(reader);
    fclose(fp);

    // String* str = string_new();

    // char* next = "def";
    // string_appendString(str, next, strlen(next));
    // printf("%s\n", string_getString(str));

    return 0;
}
