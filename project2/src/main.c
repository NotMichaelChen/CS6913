#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "reader/reader.h"
#include "reader/dirreader.h"
#include "michaellib/string.h"
#include "intermfilegen/filegen.h"
#include "mergesort/mergesort.h"
#include "indexbuilder/naive/indexer.h"
#include "indexbuilder/pagetable.h"

int main(int argc, char *argv[]) {

    if(argc != 4) {
        printf("Usage: ./indexer [buffersize] [WETdir] [OutputDir]\n");
    }

    else {
        size_t buffer = strtoull(argv[1], NULL, 0);
        char* WET = argv[2];
        char* output = argv[3];

        struct timeval t1, t2;
        double elapsedtime;
        gettimeofday(&t1, NULL);

        DirReader* reader = dirreader_new(WET);
        PostingGenerator* postinggen = postinggen_new(output, buffer);
        PageTable* table = pagetable_new();
        Document doc;

        printf("Generating intermediate files...\n");

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

        gettimeofday(&t2, NULL);
        elapsedtime = (t2.tv_sec - t1.tv_sec);
        printf("Merging intermediate files... (%fs)\n", elapsedtime);

        merge(output, "merged", buffer);

        gettimeofday(&t2, NULL);
        elapsedtime = (t2.tv_sec - t1.tv_sec);
        printf("Building final index... (%fs)\n", elapsedtime);

        Lexicon* lex = naive_buildIndex(output, "merged", "index");
        

        FILE* lfp = fopen("output/lexicon", "wb");
        lexicon_dump(&lex, lfp);
        fclose(lfp);
        lexicon_free(&lex);

        FILE* tfp = fopen("output/pagetable", "wb");
        pagetable_dump(table, tfp);
        fclose(tfp);
        pagetable_free(table);

        gettimeofday(&t2, NULL);
        elapsedtime = (t2.tv_sec - t1.tv_sec);
        printf("Done (%fs)\n", elapsedtime);
    }

    return 0;
}
