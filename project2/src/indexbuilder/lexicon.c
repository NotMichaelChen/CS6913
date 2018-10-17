#include "lexicon.h"

#include "lib/uthash.h"

struct Lexicon {
    char* key;
    size_t val;
    UT_hash_handle hh;
};

Lexicon* lexicon_new() {
    return NULL;
}

void lexicon_insert(Lexicon** lex, char* term, size_t termlen, size_t pos) {
    Lexicon* ent;

    HASH_FIND_STR(*lex, term, ent);

    if(ent == NULL) {
        ent = malloc(sizeof(Lexicon));
        ent->key = malloc(termlen+1);
        strcpy(ent->key, term);
        ent->val = pos;
        HASH_ADD_KEYPTR(hh, *lex, ent->key, termlen, ent);
    }
    // Do nothing if term already encountered (should not happen)
    //TODO: maybe raise some error here?
}

size_t lexicon_get(Lexicon** lex, char* term) {
    Lexicon* ent;
    HASH_FIND_STR(*lex, term, ent);

    if(ent == NULL) {

    }

    return ent->val;
}

void lexicon_dump(Lexicon** lex, FILE* fp) {

    Lexicon* iter;
    Lexicon* tmp;
    HASH_ITER(hh, *lex, iter, tmp) {
        fputs(iter->key, fp);
        fputc(0, fp);
        fwrite(&iter->val, sizeof(iter->val), 1, fp);

        HASH_DEL(*lex, iter);

        free(iter->key);
        free(iter);
    }
}

void lexicon_free(Lexicon** lex) {
    Lexicon* iter;
    Lexicon* tmp;

    HASH_ITER(hh, *lex, iter, tmp) {
        HASH_DEL(*lex, iter);

        free(iter->key);
        free(iter);
    }
}