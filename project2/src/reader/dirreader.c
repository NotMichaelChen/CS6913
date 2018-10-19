#include "dirreader.h"

#include <stdlib.h>
#include <string.h>

#include "dirent.h"
#include "michaellib/string.h"

struct DirReader {
    // Dirent structs to traverse directory
    struct dirent* ent;
    DIR* dir;

    // File pointer that the Reader object will use to read from
    FILE* fp;
    Reader* docreader;
    int status;
    // Represents the directory name to read from
    char* directory;
};

DirReader* dirreader_new(char* directory) {
    DirReader* reader = malloc(sizeof(DirReader));

    reader->directory = directory;

    // If the directory can't be opened, set error
    reader->dir = opendir(directory);
    if(reader->dir == NULL) {
        reader->status = 1;
        return reader;
    }

    // Keep trying to read a directory as long as the dirent object isn't null
    // and the directory isn't the ".." or "." folders
    do {
        reader->ent = readdir(reader->dir);
        if(reader->ent == NULL) {
            reader->status = 4;
            return reader;
        }
    } while(!strcmp(reader->ent->d_name,".") || !strcmp(reader->ent->d_name,".."));

    // Construct the path string used to open the file
    String* str = string_newstr(directory);
    string_appendString(str, "/", 1);
    string_appendString(str, reader->ent->d_name, strlen(reader->ent->d_name));

    reader->fp = fopen(string_getString(str), "r");

    string_free(str);

    // If the file pointer failed in opening the file, set error
    if(!reader->fp) {
        reader->status = 2;
        return reader;
    }

    reader->docreader = reader_new(reader->fp);
    // If the reader object unsuccessfully loaded the file, set error
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

    // Loop until the document is valid. The status code is stored in the err
    // variable
    do {
        // Try to read the document and get the status
        doc = reader_getDocument(reader->docreader);

        err = reader_getStatus(reader->docreader);

        // Only run if there's an error reading the next document
        if(err) {

            // Find the next valid directory (not "." or "..")
            do {
                reader->ent = readdir(reader->dir);
                if(reader->ent == NULL) {
                    reader->status = 5;
                    return doc;
                }
            } while(!strcmp(reader->ent->d_name,".") || !strcmp(reader->ent->d_name,".."));

            // Free the old reader and close its associated file pointer
            reader_free(reader->docreader);
            fclose(reader->fp);

            // Construct the filepath for the next file to read
            String* str = string_newstr(reader->directory);
            string_appendString(str, "/", 1);
            string_appendString(str, reader->ent->d_name, strlen(reader->ent->d_name));

            // Open the filepath
            reader->fp = fopen(string_getString(str), "r");

            string_free(str);

            // Set error if unable to open filepath
            if(!reader->fp) {
                reader->status = 6;
                return doc;
            }

            reader->docreader = reader_new(reader->fp);
            // Set error if unable to read file
            if(reader_getStatus(reader->docreader)) {
                reader->status = 7;
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