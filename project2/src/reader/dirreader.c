#include "dirreader.h"

#include <stdlib.h>
#include <string.h>

#include "dirent.h"
#include "michaellib/string.h"

struct DirReader {
    struct dirent* ent;
    DIR* dir;

    FILE* fp;
    Reader* docreader;
    int status;
    char* directory;
};

DirReader* dirreader_new(char* directory) {
    DirReader* reader = malloc(sizeof(DirReader));

    reader->directory = directory;

    reader->dir = opendir(directory);
    if(reader->dir == NULL) {
        reader->status = 1;
        return reader;
    }

    reader->ent = readdir(reader->dir);
    if(reader->ent == NULL) {
        reader->status = 4;
        return reader;
    }

    String* str = string_newstr(directory);
    string_appendString(str, "/", 1);
    string_appendString(str, reader->ent->d_name, strlen(reader->ent->d_name));
    reader->fp = fopen(string_getString(str), "r");
    string_free(str);

    if(!reader->fp) {
        reader->status = 2;
        return reader;
    }

    reader->docreader = reader_new(reader->fp);
    if(reader_getStatus(reader->docreader)) {
        reader->status = 3;
        return reader;
    }

    reader->status = 0;
    return reader;
}

Document dirreader_getDocument(DirReader* reader) {
    int err = 0;
    Document doc;
    do {
        doc = reader_getDocument(reader->docreader);

        err = reader_getStatus(reader->docreader);

        if(err) {
            reader->ent = readdir(reader->dir);
            if(reader->ent == NULL) {
                reader->status = 1;
                return doc;
            }

            reader_free(reader->docreader);
            fclose(reader->fp);

            reader->fp = fopen(reader->ent->d_name, "r");
            if(!reader->fp) {
                reader->status = 2;
                return doc;
            }

            reader->docreader = reader_new(reader->fp);
            if(!reader_getStatus(reader->docreader)) {
                reader->status = 3;
                return doc;
            }
        }
    } while(err);

    return doc;
}

int dirreader_getStatus(DirReader* reader) {
    return reader->status;
}

void dirreader_free(DirReader* reader) {
    closedir(reader->dir);
    fclose(reader->fp);
    reader_free(reader->docreader);

    free(reader);
}