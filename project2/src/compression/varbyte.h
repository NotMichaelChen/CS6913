#pragma once

#include <stdint.h>

#include "michaellib/string.h"

//Use String to encode byte arrays for convenience (we get length and free func)
String* varbyte_encode(uint64_t num);

uint64_t varbyte_decode(char* bytestream);