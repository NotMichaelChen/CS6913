#pragma once

#include <stdlib.h>

typedef struct String String;

String* string_new();
String* string_newstr(char* str);

char* string_getString(String* str);
size_t string_getLen(String* str);

void string_appendString(String* str, char* toappend, size_t len);

void string_free(String* str);