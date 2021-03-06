#pragma once

#include <stdlib.h>

typedef struct String String;

String* string_newstr(char* str);

char* string_getString(String* str);
size_t string_getLen(String* str);

//len does not include the null terminator (result of strlen)
void string_appendString(String* str, char* toappend, size_t len);

void string_free(String* str);