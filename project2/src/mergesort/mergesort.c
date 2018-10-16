#include "mergesort.h"

#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "michaellib/string.h"

int sort(char* directory) {
    DIR *dir;
    struct dirent *ent;
    if((dir = opendir("output")) != NULL) {
        while((ent = readdir(dir)) != NULL) {
            if(!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) {
                continue;
            }

            String* command = string_newstr("sort --parallel=4 -S 1G -k1,1 -k2n ");

            String* path = string_newstr(directory);
            string_appendString(path, "/", 1);
            string_appendString(path, ent->d_name, strlen(ent->d_name));

            string_appendString(command, string_getString(path), string_getLen(path));
            string_appendString(command, " -o ", 4);
            string_appendString(command, string_getString(path), string_getLen(path));

            printf("Sorting %s\n", ent->d_name);
            system(string_getString(command));

            string_free(command);
            string_free(path);
        }

        closedir(dir);
    }
    else {
        return 1;
    }

    return 0;
}

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