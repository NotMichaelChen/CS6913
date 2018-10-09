#include "string.h"

#include <string.h>
#include <stdio.h>

struct String {
    size_t len;
    char* buf;
};

String* string_new() {
    String* str = malloc(sizeof(String));
    str->len = 0;
    str->buf = NULL;

    return str;
}

String* string_newstr(char* str) {
    String* newstr = malloc(sizeof(String));

    newstr->len = strlen(str);
    newstr->buf = malloc(newstr->len + 1);

    strncpy(newstr->buf, str, newstr->len + 1);

    return newstr;
}

char* string_getString(String* str) {
    return str->buf;
}

size_t string_getLen(String* str) {
    return str->len;
}

void string_appendString(String* str, char* toappend, size_t len) {
    if(str->len == 0) {
        free(str->buf);

        str->len = len;
        str->buf = malloc(str->len + 1);

        strncpy(str->buf, toappend, str->len + 1);
    }
    else if(len == 0) {
        // do nothing
    }
    else {
        char* newbuf = malloc(str->len + len + 1);
        snprintf(newbuf, str->len + len + 1, "%s%s", str->buf, toappend);

        free(str->buf);
        str->buf = newbuf;
        str->len = str->len + len;
    }
}

void string_free(String* str) {
    free(str->buf);
    free(str);
}