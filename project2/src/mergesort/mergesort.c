#include "mergesort.h"

#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "michaellib/string.h"

void eraseIntermediates(char* directory, char* outputname) {
    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(directory)) != NULL) {
        while((ent = readdir(dir)) != NULL) {

            if(!strcmp(ent->d_name, ".")
                || !strcmp(ent->d_name, "..")
                || !strcmp(ent->d_name, outputname))
            {
                continue;
            }

            String* path = string_newstr(directory);
            string_appendString(path, "/", 1);
            string_appendString(path, ent->d_name, strlen(ent->d_name));

            remove(string_getString(path));

            string_free(path);
        }

        closedir(dir);
    }
}

int merge(char* directory, char* outputname) {
    String* dirstr = string_newstr(directory);
    String* command = string_newstr("LC_ALL=C sort -m -k1,1 -k2n ");

    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(directory)) != NULL) {
        while((ent = readdir(dir)) != NULL) {
            if(!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) {
                continue;
            }

            string_appendString(command, string_getString(dirstr), string_getLen(dirstr));
            string_appendString(command, "/", 1);
            string_appendString(command, ent->d_name, strlen(ent->d_name));
            string_appendString(command, " ", 1);
        }

        string_appendString(command, "-o ", 3);
        string_appendString(command, string_getString(dirstr), string_getLen(dirstr));
        string_appendString(command, "/", 1);
        string_appendString(command, outputname, strlen(outputname));

        system(string_getString(command));

        closedir(dir);
        eraseIntermediates(directory, outputname);
    }
    else {
        string_free(dirstr);
        string_free(command);
        return 1;
    }

    string_free(dirstr);
    string_free(command);
    return 0;
}