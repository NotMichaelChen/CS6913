#include "varbyte.h"

#include <string.h>

//Little endian encoding
inline uint8_t* varbyte_encode(uint64_t num, size_t* len) {
    //Need at most 10 7-bit bytes to represent a 64-bit number
    //Initialize to zero to make converting to string easier
    uint8_t bytestream[10] = {0};

    uint8_t index = 0;
    while(1) {
        bytestream[index] = num & 127;
        if(num < 128)
            break;
        num >>= 7;
        ++index;
    }

    bytestream[index] += 128;
    *len = index+1;
    uint8_t* res = malloc(*len);
    memcpy(res, bytestream, *len);
    return res;
}

inline ByteVec* varbyte_encodeblock(uint64_t* nums, size_t count) {
    ByteVec* vec = bytevec_new();

    uint8_t* compressednum;
    size_t comprlen = 0;

    for(size_t i = 0; i < count; i++) {
        compressednum = varbyte_encode(nums[i], &comprlen);
        bytevec_appendRange(vec, compressednum, comprlen);
        free(compressednum);
    }

    return vec;
}

inline uint64_t varbyte_decode(uint8_t* bytestream) {
    uint64_t num = 0;
    uint8_t shiftamt = 0;
    uint8_t* walker = (uint8_t*) bytestream;
    while(*walker < 128) {
        num += *walker << shiftamt;
        shiftamt += 7;
        ++walker;
    }

    num += (*walker & 127) << shiftamt;

    return num;
}

inline ULongVector* varbyte_decodeStream(uint8_t* bytestream, size_t len) {
    ULongVector* vec = ulongvector_new();

    uint8_t* walker = bytestream;

    while(walker - bytestream < len) {
        uint64_t num = 0;
        uint8_t shiftamt = 0;

        while(*walker < 128) {
            num += *walker << shiftamt;
            shiftamt += 7;
            walker++;
        }

        num += (*walker & 127) << shiftamt;
        walker++;

        ulongvector_append(vec, num);
    }

    return vec;
}