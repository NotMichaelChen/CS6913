#pragma once

#include <stdint.h>

#include "michaellib/string.h"
#include "michaellib/bytevec.h"

//Use String to encode byte arrays for convenience (we get length and free func)
String* varbyte_encode(uint64_t num);
ByteVec* varbyte_encodeblock(uint64_t* nums, size_t count);

uint64_t varbyte_decode(char* bytestream);