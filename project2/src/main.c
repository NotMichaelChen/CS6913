#include <stdio.h>
#include <string.h>
#include <time.h>

#include "reader/reader.h"
#include "reader/dirreader.h"
#include "michaellib/string.h"
#include "intermfilegen/filegen.h"
#include "mergesort/mergesort.h"
#include "indexbuilder/naive/indexer.h"

#include "compression/varbyte.h"

int main(int argc, char *argv[]) {
    printf("Hello World!\n");

    DirReader* reader = dirreader_new("CC");
    PostingGenerator* postinggen = postinggen_new("output", 100000000);
    Document doc;

    printf("Generating intermediate files... (%fs)\n", clock() / (double)CLOCKS_PER_SEC);

    int i = 0;
    while(1) {
        doc = dirreader_getDocument(reader);
        if(dirreader_getStatus(reader))
            break;
        
        postinggen_addDoc(postinggen, doc);
        
        reader_freedoc(&doc);
        i++;
    }

    postinggen_flush(postinggen);
    postinggen_free(postinggen);
    dirreader_free(reader);

    printf("Sorting intermediate files... (%fs)\n", clock() / (double)CLOCKS_PER_SEC);

    merge("output", "merged");

    printf("Building final index... (%fs)\n", clock() / (double)CLOCKS_PER_SEC);

    Lexicon* lex = naive_buildIndex("output", "merged", "index");
    FILE* lfp = fopen("output/lexicon", "wb");
    lexicon_dump(&lex, lfp);
    fclose(lfp);

    lexicon_free(&lex);

    printf("Done (%fs)\n", clock() / (double)CLOCKS_PER_SEC);

    return 0;
}
