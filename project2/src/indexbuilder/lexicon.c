#include "lexicon.h"

#include "lib/uthash.h"

typedef struct {
    char* key;
    size_t pos;
    size_t metasize;
    // How many documents are in the posting list
    size_t listlen;
    UT_hash_handle hh;
} LexiconEntry;

struct Lexicon {
    LexiconEntry* dict;
};

Lexicon* lexicon_new() {
    Lexicon* lex = malloc(sizeof(Lexicon));
    lex->dict = NULL;
    return lex;
}

void lexicon_insert(Lexicon* lex, char* term, size_t termlen, size_t pos, size_t metasize, size_t docscontaining) {
    LexiconEntry* ent;

    HASH_FIND_STR(lex->dict, term, ent);

    if(ent == NULL) {
        ent = malloc(sizeof(LexiconEntry));
        ent->key = malloc(termlen+1);
        strcpy(ent->key, term);
        ent->pos = pos;
        ent->metasize = metasize;
        ent->listlen = docscontaining;
        HASH_ADD_KEYPTR(hh, lex->dict, ent->key, termlen, ent);
    }
    // Do nothing if term already encountered (should not happen)
    //TODO: maybe raise some error here?
}

size_t lexicon_getpos(Lexicon* lex, char* term) {
    LexiconEntry* ent;
    HASH_FIND_STR(lex->dict, term, ent);

    if(ent == NULL) {
        return SIZE_MAX;
    }

    return ent->pos;
}

size_t lexicon_getmetasize(Lexicon* lex, char* term) {
    LexiconEntry* ent;
    HASH_FIND_STR(lex->dict, term, ent);

    if(ent == NULL) {
        return SIZE_MAX;
    }

    return ent->metasize;
}

size_t lexicon_getlistlen(Lexicon* lex, char* term) {
    LexiconEntry* ent;
    HASH_FIND_STR(lex->dict, term, ent);

    if(ent == NULL) {
        return SIZE_MAX;
    }

    return ent->listlen;
}

void lexicon_read(Lexicon* lex, FILE* fp) {
    char* line = NULL;
    size_t linelen = 0;
    int read = 0;

    while ((read = getline(&line, &linelen, fp)) != -1) {
        LexiconEntry* ent = malloc(sizeof(LexiconEntry));

        size_t index = 0;
        while(line[index] != 0) index++;
        size_t termlen = index;
        
        ent->key = malloc(index + 1);
        strcpy(ent->key, line);

        //Now past the \0
        index++;

        memcpy(&ent->pos, line[index], sizeof (ent->pos));
        index += sizeof (ent->pos);

        memcpy(&ent->pos, line[index], sizeof (ent->metasize));
        index += sizeof (ent->metasize);

        memcpy(&ent->pos, line[index], sizeof (ent->listlen));
        index += sizeof (ent->listlen);

        HASH_ADD_KEYPTR(hh, lex->dict, ent->key, termlen, ent);
    }

    free(line);
}

void lexicon_dump(Lexicon* lex, FILE* fp) {

    LexiconEntry* iter = NULL;
    LexiconEntry* tmp = NULL;

    HASH_ITER(hh, lex->dict, iter, tmp) {
        fputs(iter->key, fp);
        fputc(0, fp);
        //TODO: compress?
        fwrite(&iter->pos, sizeof(iter->pos), 1, fp);
        fwrite(&iter->metasize, sizeof(iter->metasize), 1, fp);
        fwrite(&iter->listlen, sizeof(iter->listlen), 1, fp);

        HASH_DEL(lex->dict, iter);

        free(iter->key);
        free(iter);
    }
}

void lexicon_free(Lexicon* lex) {
    LexiconEntry* iter = NULL;
    LexiconEntry* tmp = NULL;

    HASH_ITER(hh, lex->dict, iter, tmp) {
        HASH_DEL(lex->dict, iter);

        free(iter->key);
        free(iter);
    }

    free(lex);
}