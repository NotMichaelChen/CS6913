#include "varbyte.h"

//Little endian encoding
inline String* varbyte_encode(uint64_t num) {
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
    return string_newstr((char *) bytestream);
}

inline uint64_t varbyte_decode(char* bytestream) {
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