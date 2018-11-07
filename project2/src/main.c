#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

#include "reader/reader.h"
#include "reader/dirreader.h"
#include "michaellib/string.h"
#include "michaellib/vector/stringvec.h"
#include "intermfilegen/filegen.h"
#include "mergesort/mergesort.h"
#include "indexbuilder/indexer.h"
#include "indexbuilder/pagetable.h"
#include "query/DAAT.h"
#include "michaellib/utility.h"
#include "query/disjunctive.h"
#include "compression/varbyte.h"

int indexer(int argc, char* argv[]) {
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
        for(int i = 0; i < 10000; i++) {
            doc = dirreader_getDocument(reader);
            if(dirreader_getStatus(reader))
                break;
            
            size_t termcount = postinggen_addDoc(postinggen, doc);
            pagetable_add(table, doc.url, termcount, doc.wetpath, doc.docsize, doc.offset);
            
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
        String* lexpath = string_newstr(output);
        string_appendString(lexpath, "/lexicon", 8);
        FILE* lfp = fopen(string_getString(lexpath), "wb");
        lexicon_dump(lex, lfp);
        fclose(lfp);
        lexicon_free(lex);
        string_free(lexpath);

        String* tablepath = string_newstr(output);
        string_appendString(tablepath, "/pagetable", 10);
        FILE* tfp = fopen(string_getString(tablepath), "wb");
        pagetable_dump(table, tfp);
        fclose(tfp);
        pagetable_free(table);
        string_free(tablepath);

        gettimeofday(&t2, NULL);
        elapsedtime = (t2.tv_sec - t1.tv_sec);
        printf("Done (%fs)\n", elapsedtime);
    }

    return 0;
}

int query(int argc, char* argv[]) {
    if(argc != 3) {
        printf("Usage: ./query [indexdir] [WETdir]\n");
    }
    else {
        Lexicon* lex = lexicon_new();
        PageTable* pagetable = pagetable_new();

        String* lexpath = string_newstr(argv[1]);
        string_appendString(lexpath, "/lexicon", 8);
        FILE* lexfp = fopen(string_getString(lexpath), "rb");
        string_free(lexpath);

        String* tablepath = string_newstr(argv[1]);
        string_appendString(tablepath, "/pagetable", 10);
        FILE* tablefp = fopen(string_getString(tablepath), "rb");
        string_free(tablepath);

        printf("Loading lexicon...\n");
        lexicon_read(lex, lexfp);
        fclose(lexfp);

        printf("Loading page table...\n");
        pagetable_read(pagetable, tablefp);
        fclose(tablefp);

        String* indexpath = string_newstr(argv[1]);
        string_appendString(indexpath, "/index", 6);

        while(1) {
            printf("Enter query terms: ");

            //Get user input and tokenize it by whitespace
            char* line = NULL;
            size_t linelen = 0;

            getline(&line, &linelen, stdin);

            StringVec* strvec = stringvec_new();
            char* linewalker = strtok(line, "\t\r\n ");
            while(linewalker != NULL) {
                
                stringvec_append(strvec, linewalker);
                linewalker = strtok(NULL, "\t\r\n ");
            }

            //Ask user to select type of query
            printf("(1) Conjunctive\n(2) Disjunctive\n(3) Exit\nEnter type of query: ");

            int res = 0;
            char* numinput = NULL;
            getline(&numinput, &linelen, stdin);
            res = strtol(numinput, NULL, 10);

            MinHeap* queryheap = NULL;

            if(res == 1) {
                queryheap = DAAT(
                    stringvec_getbuffer(strvec),
                    stringvec_len(strvec),
                    lex,
                    pagetable,
                    indexpath
                );
            }
            else if(res == 2) {
                queryheap = disjunctive_query(
                    stringvec_getbuffer(strvec),
                    stringvec_len(strvec),
                    lex,
                    pagetable,
                    indexpath
                );
            }
            else if(res == 3) {
                break;
            }
            else {
                printf("Error: invalid query type\n");
                continue;
            }

            //Sort minheap array
            HeapEntry* resarr = minheap_getArr(queryheap);
            qsort(resarr, minheap_len(queryheap), sizeof(HeapEntry), util_gcmpHeapEntry);

            //Print array
            for(size_t i = 0; i < minheap_len(queryheap); i++) {
                printf("(%zu) %s\n\tscore: %lf\n\n",
                    i,
                    string_getString(pagetable_geturl(pagetable, resarr[i].docID)),
                    resarr[i].score
                );

                // Get document and print it out
                char* document = pagetable_getDocument(pagetable, resarr[i].docID);
                util_printSnippet(document, stringvec_getstr(strvec, 0));
                free(document);
            }

            minheap_free(queryheap);

            stringvec_free(strvec);
            free(line);
            free(numinput);
        }

    }
    
    return 0;
}

int main(int argc, char *argv[]) {
    if(argc == 5)
        return indexer(argc, argv);
    else if(argc == 3)
        return query(argc, argv);
    else {
        printf("Error, argument count doesn't match index or query\n");
        printf("Indexer Usage: ./index [postingbuffersize] [mergebuffersize] [WETdir] [OutputDir]\n");
        printf("Query Usage: ./index [indexdir] [WETdir]\n");
    }
    
    return 0;
}
