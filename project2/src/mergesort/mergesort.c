#include "mergesort.h"

#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "michaellib/string.h"

int merge(char* directory, char* outputname) {
    String* dirstr = string_newstr(directory);
    String* command = string_newstr("sort -m ");

    DIR *dir;
    struct dirent *ent;
    if((dir = opendir("output")) != NULL) {
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