#include "indexbuilder.h"

#include "global.h"
#include "michaellib/string.h"
#include "docwriter.h"
#include "lexicon.h"

struct IndexBuilder {
    Lexicon* lex;
    //Keeps track of how many files are written per layer
    char filetracker[LAYERCOUNT];

    size_t nextdocID;
    char* dir;
    int err;
};

IndexBuilder* indexbuilder_new(char* directory) {

}

void indexbuilder_addDoc(IndexBuilder* builder, Document doc) {
    //Write document to file
    LexiconEntry* lexent = lexicon_get(builder->lex, 0, builder->filetracker[0]+1);

    String* filename = string_newstr(builder->dir);
    char indexnum[32];
    snprintf(indexnum, 32, "%d-%d", 0, builder->filetracker[0]+1);
    string_appendString(filename, indexnum, strlen(indexnum));
    FILE* fp = fopen(filename, "w");
    string_free(filename);

    docwriter_writeDoc(doc, builder->nextdocID, fp, lexent);

    builder->nextdocID += 1;
    builder->filetracker[0] += 1;

    //Update page table

    //If filenum > n, call merge function
    if(builder->filetracker[0] >= FILECOUNT) {
        
    }
}
void indexbuilder_mergeAll(IndexBuilder* builder);

void indexbuilder_free(IndexBuilder* builder)