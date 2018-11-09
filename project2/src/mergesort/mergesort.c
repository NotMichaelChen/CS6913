#include "mergesort.h"

#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "michaellib/string.h"

// Helper function that erases everything in a directory other than "outputname"
void eraseIntermediates(char* directory, char* outputname) {
    DIR *dir;
    if((dir = opendir(directory)) != NULL) {
        struct dirent *ent;
        // Iterate through the directory
        while((ent = readdir(dir)) != NULL) {

            // Skip files with the outputname, ".", or ".."
            if(!strcmp(ent->d_name, ".")
                || !strcmp(ent->d_name, "..")
                || !strcmp(ent->d_name, outputname))
            {
                continue;
            }

            // Construct the filepath
            String* path = string_newstr(directory);
            string_appendString(path, "/", 1);
            string_appendString(path, ent->d_name, strlen(ent->d_name));

            // Delete the file
            remove(string_getString(path));

            string_free(path);
        }

        closedir(dir);
    }
}

int merge(char* directory, char* outputname, size_t buffer) {
    String* dirstr = string_newstr(directory);
    // Create the command
    String* command = string_newstr("LC_ALL=C sort -m -k1,1 -k2n -T \"");
    string_appendString(command, string_getString(dirstr), string_getLen(dirstr));
    string_appendString(command, "\" -S ", 5);
    
    // Add the buffer size argument to the merge command
    //64-bit number = 19 chars, null terminator, "b", space. Sums up to 22 bytes
    char bufsize[22];
    // buffer argument is a number followed by b e.g. 1000b = 1kb
    sprintf(bufsize, "%zub ", buffer);
    string_appendString(command, bufsize, strlen(bufsize));

    DIR *dir;
    if((dir = opendir(directory)) != NULL) {
        struct dirent *ent;
        // Iterate through directory
        while((ent = readdir(dir)) != NULL) {
            // Skip "." and ".."
            if(!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) {
                continue;
            }

            // Construct the filepath and add it to the command
            string_appendString(command, string_getString(dirstr), string_getLen(dirstr));
            string_appendString(command, "/", 1);
            string_appendString(command, ent->d_name, strlen(ent->d_name));
            string_appendString(command, " ", 1);
        }

        // Add the output file
        string_appendString(command, "-o ", 3);
        string_appendString(command, string_getString(dirstr), string_getLen(dirstr));
        string_appendString(command, "/", 1);
        string_appendString(command, outputname, strlen(outputname));

        // Call the merge command
        int res = system(string_getString(command));
        printf("Merge result returned %d\n", res);

        // Close directory and erase the intermediate files
        closedir(dir);
        //eraseIntermediates(directory, outputname);
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