#include <stdio.h>
#include <string.h>
#include <time.h>

#include "reader/reader.h"
#include "reader/dirreader.h"
#include "michaellib/string.h"
#include "intermfilegen/filegen.h"
#include "mergesort/mergesort.h"
#include "indexbuilder/naive/indexer.h"
#include "indexbuilder/pagetable.h"

int main(int argc, char *argv[]) {
    printf("Hello World!\n");

    DirReader* reader = dirreader_new("CC");
    PostingGenerator* postinggen = postinggen_new("output", 1000000000);
    PageTable* table = pagetable_new();
    Document doc;

    printf("Generating intermediate files... (%fs)\n", clock() / (double)CLOCKS_PER_SEC);

    int i = 0;
    while(1) {
        doc = dirreader_getDocument(reader);
        if(dirreader_getStatus(reader))
            break;
        
        pagetable_add(table, doc.url, doc.docsize);
        postinggen_addDoc(postinggen, doc);
        
        reader_freedoc(&doc);
        i++;
    }
    
    postinggen_flush(postinggen);
    postinggen_free(postinggen);
    dirreader_free(reader);

    printf("Merging intermediate files... (%fs)\n", clock() / (double)CLOCKS_PER_SEC);

    merge("output", "merged");

    printf("Building final index... (%fs)\n", clock() / (double)CLOCKS_PER_SEC);

    Lexicon* lex = naive_buildIndex("output", "merged", "index");
    
    FILE* lfp = fopen("output/lexicon", "wb");
    lexicon_dump(&lex, lfp);
    fclose(lfp);
    lexicon_free(&lex);

    FILE* tfp = fopen("output/pagetable", "wb");
    pagetable_dump(table, tfp);
    fclose(tfp);
    pagetable_free(table);

    printf("Done (%fs)\n", clock() / (double)CLOCKS_PER_SEC);

    return 0;
}
