#pragma once

#include <stdio.h>
#include <stdlib.h>

typedef struct StringVec StringVec;

StringVec* stringvec_new();
void stringvec_append(StringVec* stringvec, char* str);
size_t stringvec_len(StringVec* stringvec);
char* stringvec_getstr(StringVec* stringvec, size_t index);
char** stringvec_getbuffer(StringVec* stringvec);

//Assume we have no responsibilities for the given data
void stringvec_free(StringVec* stringvec);