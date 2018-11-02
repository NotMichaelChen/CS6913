#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "reader/reader.h"
#include "reader/dirreader.h"
#include "michaellib/string.h"
#include "intermfilegen/filegen.h"
#include "mergesort/mergesort.h"
#include "indexbuilder/indexer.h"
#include "indexbuilder/pagetable.h"

int main(int argc, char *argv[]) {

    if(argc != 5) {
        printf("Usage: ./indexer [postingbuffersize] [mergebuffersize] [WETdir] [OutputDir]\n");
    }

    else {
        size_t postingbuffer = strtoull(argv[1], NULL, 0);
        size_t mergebuffer = strtoull(argv[2], NULL, 0);
        char* WET = argv[3];
        char* output = argv[4];

        struct timeval t1, t2;
        double elapsedtime;
        gettimeofday(&t1, NULL);

        // Initialze document reader, posting generator, and page table
        DirReader* reader = dirreader_new(WET);
        PostingGenerator* postinggen = postinggen_new(output, postingbuffer);
        PageTable* table = pagetable_new();
        Document doc;

        printf("Generating intermediate files...\n");
    
        // Add documents to the posting generator until there's no more documents
        while(1) {
            doc = dirreader_getDocument(reader);
            if(dirreader_getStatus(reader))
                break;
            
            pagetable_add(table, doc.url, doc.docsize);
            postinggen_addDoc(postinggen, doc);
            
            reader_freedoc(&doc);
        }
        
        // Empty out any postings left in the posting generator
        postinggen_flush(postinggen);
        postinggen_free(postinggen);
        dirreader_free(reader);

        gettimeofday(&t2, NULL);
        elapsedtime = (t2.tv_sec - t1.tv_sec);
        printf("Merging intermediate files... (%fs)\n", elapsedtime);

        // Merge all files output by the posting generator
        merge(output, "merged", mergebuffer);

        gettimeofday(&t2, NULL);
        elapsedtime = (t2.tv_sec - t1.tv_sec);
        printf("Building final index... (%fs)\n", elapsedtime);

        // Build the index, which produces a lexicon
        Lexicon* lex = block_buildIndex(output, "merged", "index");
        
        // Write out the lexicon and pagetable
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
